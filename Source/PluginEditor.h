#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VocalSaturatorAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:
    explicit VocalSaturatorAudioProcessorEditor(
        VocalSaturatorAudioProcessor& processor);

    ~VocalSaturatorAudioProcessorEditor() override = default;

    void paint(juce::Graphics& graphics) override;
    void resized() override;

private:
    VocalSaturatorAudioProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Label driveLabel;

    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        VocalSaturatorAudioProcessorEditor)
};