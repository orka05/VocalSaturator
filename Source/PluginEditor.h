
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VocalSaturatorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VocalSaturatorLookAndFeel();

    void drawRotarySlider(
        juce::Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;

    juce::Slider::SliderLayout getSliderLayout(
        juce::Slider& slider) override;

    juce::Label* createSliderTextBox(
        juce::Slider& slider) override;
};

class VocalSaturatorAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:
    explicit VocalSaturatorAudioProcessorEditor(
        VocalSaturatorAudioProcessor& p);

    ~VocalSaturatorAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    VocalSaturatorAudioProcessor& audioProcessor;

    VocalSaturatorLookAndFeel customLookAndFeel;
    juce::TooltipWindow tooltipWindow;

    juce::Label titleLabel;
    juce::Label subtitleLabel;

    juce::Slider driveSlider;
    juce::Slider mixSlider;
    juce::Slider toneSlider;
    juce::Slider outputSlider;

    juce::Label driveLabel;
    juce::Label mixLabel;
    juce::Label toneLabel;
    juce::Label outputLabel;

    juce::Label driveUnitLabel;
    juce::Label mixUnitLabel;
    juce::Label toneUnitLabel;
    juce::Label outputUnitLabel;

    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> toneAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        VocalSaturatorAudioProcessorEditor)
};
