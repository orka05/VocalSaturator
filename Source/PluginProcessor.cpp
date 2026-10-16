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

    return layout;
}

void VocalSaturatorAudioProcessor::prepareToPlay(
    double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
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

    const float driveDb = driveParameter->load();
    const float gain = juce::Decibels::decibelsToGain(driveDb);
    const float normalizer = 1.0f / std::tanh(gain);

    const int numChannels = juce::jmin(
        getTotalNumInputChannels(),
        buffer.getNumChannels()
    );

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float input = samples[sample];
            samples[sample] =
                std::tanh(input * gain) * normalizer;
        }
    }

    for (int channel = numChannels;
        channel < buffer.getNumChannels(); ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
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