#pragma once

#include <JuceHeader.h>
#include <array>

class VocalSaturatorAudioProcessor : public juce::AudioProcessor
{
public:
    VocalSaturatorAudioProcessor();
    ~VocalSaturatorAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>& buffer,
        juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& name) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

    std::atomic<float>* driveParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* outputParameter = nullptr;
    std::atomic<float>* toneParameter = nullptr;


    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
        smoothedDrive;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
        smoothedMix;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
        smoothedOutput;
    juce::SmoothedValue<float,
        juce::ValueSmoothingTypes::Linear> smoothedTone;

    // Separate filter state for left and right channels.
    std::array<juce::IIRFilter, 2> lowShelfFilters;
    std::array<juce::IIRFilter, 2> highShelfFilters;

    double currentSampleRate = 44100.0;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    juce::AudioBuffer<float> wetBuffer;
    juce::AudioBuffer<float> dryDelayBuffer;

    int dryDelayWritePosition = 0;
    int oversamplingLatency = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        VocalSaturatorAudioProcessor)
};