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
    juce::Slider mixSlider;
    juce::Label mixLabel;
    juce::Slider outputSlider;
    juce::Label outputLabel;
    juce::Slider toneSlider;
    juce::Label toneLabel;

    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> toneAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalSaturatorAudioProcessorEditor)
};