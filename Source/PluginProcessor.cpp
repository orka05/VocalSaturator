#include "PluginProcessor.h"
#include "PluginEditor.h"

VocalSaturatorAudioProcessor::VocalSaturatorAudioProcessor()
    : juce::AudioProcessor(
        BusesProperties()
        .withInput(
            "Input",
            juce::AudioChannelSet::stereo(),
            true
        )
        .withOutput(
            "Output",
            juce::AudioChannelSet::stereo(),
            true
        )
    )
{
}

const juce::String VocalSaturatorAudioProcessor::getName() const
{
    return "Vocal Saturator";
}

bool VocalSaturatorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool VocalSaturatorAudioProcessor::producesMidi() const
{
    return false;
}

bool VocalSaturatorAudioProcessor::isMidiEffect() const
{
    return false;
}

double VocalSaturatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalSaturatorAudioProcessor::getNumPrograms()
{
    return 1;
}

int VocalSaturatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

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
    int index,
    const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void VocalSaturatorAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void VocalSaturatorAudioProcessor::releaseResources()
{
}

bool VocalSaturatorAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto inputLayout = layouts.getMainInputChannelSet();
    const auto outputLayout = layouts.getMainOutputChannelSet();

    if (inputLayout != outputLayout)
        return false;

    return outputLayout == juce::AudioChannelSet::mono()
        || outputLayout == juce::AudioChannelSet::stereo();
}

void VocalSaturatorAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    const auto totalInputChannels = getTotalNumInputChannels();
    const auto totalOutputChannels = getTotalNumOutputChannels();

    // Clear output channels that do not have a corresponding input.
    for (auto channel = totalInputChannels;
        channel < totalOutputChannels;
        ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
    }

    // Audio currently passes through unchanged.
}

bool VocalSaturatorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor*
VocalSaturatorAudioProcessor::createEditor()
{
    return new VocalSaturatorAudioProcessorEditor(*this);
}

void VocalSaturatorAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void VocalSaturatorAudioProcessor::setStateInformation(
    const void* data,
    int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalSaturatorAudioProcessor();
}
