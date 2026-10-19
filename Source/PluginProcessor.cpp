#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

VocalSaturatorAudioProcessor::VocalSaturatorAudioProcessor()
    : juce::AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    driveParameter = parameters.getRawParameterValue("drive");
    mixParameter = parameters.getRawParameterValue("mix");
    outputParameter = parameters.getRawParameterValue("output");

}

juce::AudioProcessorValueTreeState::ParameterLayout
VocalSaturatorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "drive", 1 },
        "Drive",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "mix", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "output", 1 },
        "Output",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")
    ));


    return layout;
}

void VocalSaturatorAudioProcessor::prepareToPlay(
    double sampleRate, int samplesPerBlock)
{
    constexpr double smoothingTimeSeconds = 0.02;

    smoothedDrive.reset(sampleRate, smoothingTimeSeconds);
    smoothedMix.reset(sampleRate, smoothingTimeSeconds);
    smoothedOutput.reset(sampleRate, smoothingTimeSeconds);

    smoothedDrive.setCurrentAndTargetValue(
        driveParameter->load());

    smoothedMix.setCurrentAndTargetValue(
        mixParameter->load() / 100.0f);

    smoothedOutput.setCurrentAndTargetValue(
        outputParameter->load());

    const auto channels =
        static_cast<size_t>(getTotalNumInputChannels());

    oversampler = std::make_unique<
        juce::dsp::Oversampling<float>>(
            channels,
            2, // 2 stages: 2^2 = 4x
            juce::dsp::Oversampling<float>::
            filterHalfBandFIREquiripple,
            true,
            true // Integer latency
            );

    oversampler->initProcessing(
        static_cast<size_t>(samplesPerBlock));

    oversampler->reset();

    oversamplingLatency = juce::roundToInt(
        oversampler->getLatencyInSamples());

    setLatencySamples(oversamplingLatency);

    wetBuffer.setSize(
        static_cast<int>(channels),
        samplesPerBlock);

    dryDelayBuffer.setSize(
        static_cast<int>(channels),
        juce::jmax(1, oversamplingLatency));

    dryDelayBuffer.clear();
    dryDelayWritePosition = 0;
}

void VocalSaturatorAudioProcessor::releaseResources()
{
}

bool VocalSaturatorAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    return input == output &&
        (output == juce::AudioChannelSet::mono() ||
            output == juce::AudioChannelSet::stereo());
}

void VocalSaturatorAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    const int numChannels = getTotalNumInputChannels();
    const int numSamples = buffer.getNumSamples();

    if (numSamples == 0)
        return;

    jassert(oversampler != nullptr);
    jassert(numSamples <= wetBuffer.getNumSamples());

    if (oversampler == nullptr ||
        numSamples > wetBuffer.getNumSamples())
    {
        buffer.clear();
        return;
    }

    smoothedDrive.setTargetValue(driveParameter->load());
    smoothedMix.setTargetValue(mixParameter->load() / 100.0f);
    smoothedOutput.setTargetValue(outputParameter->load());

    // Copy input into the wet processing buffer.
    for (int channel = 0; channel < numChannels; ++channel)
    {
        wetBuffer.copyFrom(
            channel, 0, buffer, channel, 0, numSamples);
    }

    juce::dsp::AudioBlock<float> wetBlock(wetBuffer);
    auto inputBlock = wetBlock.getSubBlock(
        0, static_cast<size_t>(numSamples));

    // Upsample 4x.
    auto highRateBlock =
        oversampler->processSamplesUp(inputBlock);

    const int highRateSamples =
        static_cast<int>(highRateBlock.getNumSamples());

    // Apply the waveshaper at the higher sample rate.
    // Drive smoothing advances at the original sample rate.
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float driveDb = smoothedDrive.getNextValue();

        const float gain =
            juce::Decibels::decibelsToGain(driveDb);

        const float normalizer =
            1.0f / std::tanh(gain);

        for (int phase = 0; phase < 4; ++phase)
        {
            const int index = sample * 4 + phase;

            if (index >= highRateSamples)
                break;

            for (int channel = 0;
                channel < numChannels; ++channel)
            {
                auto* samples =
                    highRateBlock.getChannelPointer(
                        static_cast<size_t>(channel));

                samples[index] =
                    std::tanh(samples[index] * gain)
                    * normalizer;
            }
        }
    }

    // Downsample back to the host sample rate.
    oversampler->processSamplesDown(inputBlock);

    // Mix with latency-compensated dry audio.
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float mix = smoothedMix.getNextValue();

        const float outputGain =
            juce::Decibels::decibelsToGain(
                smoothedOutput.getNextValue());

        for (int channel = 0;
            channel < numChannels; ++channel)
        {
            const float input =
                buffer.getSample(channel, sample);

            float dry = input;

            if (oversamplingLatency > 0)
            {
                dry = dryDelayBuffer.getSample(
                    channel, dryDelayWritePosition);

                dryDelayBuffer.setSample(
                    channel, dryDelayWritePosition, input);
            }

            const float wet =
                wetBuffer.getSample(channel, sample);

            const float mixed =
                dry * (1.0f - mix) + wet * mix;

            buffer.setSample(
                channel, sample, mixed * outputGain);
        }

        if (oversamplingLatency > 0)
        {
            dryDelayWritePosition =
                (dryDelayWritePosition + 1)
                % oversamplingLatency;
        }
    }

    for (int channel = numChannels;
        channel < buffer.getNumChannels();
        ++channel)
    {
        buffer.clear(channel, 0, numSamples);
    }
}

juce::AudioProcessorEditor*
VocalSaturatorAudioProcessor::createEditor()
{
    return new VocalSaturatorAudioProcessorEditor(*this);
}

bool VocalSaturatorAudioProcessor::hasEditor() const { return true; }

const juce::String VocalSaturatorAudioProcessor::getName() const
{
    return "Vocal Saturator";
}

bool VocalSaturatorAudioProcessor::acceptsMidi() const { return false; }
bool VocalSaturatorAudioProcessor::producesMidi() const { return false; }
bool VocalSaturatorAudioProcessor::isMidiEffect() const { return false; }
double VocalSaturatorAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int VocalSaturatorAudioProcessor::getNumPrograms() { return 1; }
int VocalSaturatorAudioProcessor::getCurrentProgram() { return 0; }

void VocalSaturatorAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String VocalSaturatorAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void VocalSaturatorAudioProcessor::changeProgramName(
    int index, const juce::String& name)
{
    juce::ignoreUnused(index, name);
}

void VocalSaturatorAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, destData);
}

void VocalSaturatorAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalSaturatorAudioProcessor();
}