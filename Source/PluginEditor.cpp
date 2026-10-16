#include "PluginEditor.h"

VocalSaturatorAudioProcessorEditor::
VocalSaturatorAudioProcessorEditor(
    VocalSaturatorAudioProcessor& processor)
    : juce::AudioProcessorEditor(&processor),
    audioProcessor(processor)
{
    driveSlider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    driveSlider.setTextBoxStyle(
        juce::Slider::TextBoxBelow, false, 90, 24);

    driveSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(driveSlider);

    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.setColour(
        juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(driveLabel);

    driveAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, "drive", driveSlider);

    mixSlider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(
        juce::Slider::TextBoxBelow, false, 90, 24);
    mixSlider.setTextValueSuffix(" %");
    addAndMakeVisible(mixSlider);

    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.setColour(
        juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(mixLabel);

    mixAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, "mix", mixSlider);

    outputSlider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);
    outputSlider.setTextBoxStyle(
        juce::Slider::TextBoxBelow, false, 90, 24);
    outputSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(outputSlider);

    outputLabel.setText("OUTPUT", juce::dontSendNotification);
    outputLabel.setJustificationType(juce::Justification::centred);
    outputLabel.setColour(
        juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(outputLabel);

    outputAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, "output", outputSlider);

    setSize(520, 320);
}

void VocalSaturatorAudioProcessorEditor::paint(
    juce::Graphics& graphics)
{
    graphics.fillAll(juce::Colour::fromRGB(24, 26, 31));

    graphics.setColour(juce::Colours::white);
    graphics.setFont(juce::FontOptions(26.0f));

    graphics.drawFittedText(
        "VOCAL SATURATOR",
        0, 25, getWidth(), 45,
        juce::Justification::centred, 1);
}

void VocalSaturatorAudioProcessorEditor::resized()
{
    constexpr int knobWidth = 130;
    constexpr int knobHeight = 145;
    constexpr int spacing = 30;

    const int totalWidth = knobWidth * 3 + spacing * 2;
    const int startX = (getWidth() - totalWidth) / 2;
    const int knobY = 105;
    const int labelY = 250;

    driveSlider.setBounds(
        startX, knobY, knobWidth, knobHeight);
    driveLabel.setBounds(
        startX, labelY, knobWidth, 30);

    mixSlider.setBounds(
        startX + knobWidth + spacing,
        knobY, knobWidth, knobHeight);
    mixLabel.setBounds(
        startX + knobWidth + spacing,
        labelY, knobWidth, 30);

    outputSlider.setBounds(
        startX + (knobWidth + spacing) * 2,
        knobY, knobWidth, knobHeight);
    outputLabel.setBounds(
        startX + (knobWidth + spacing) * 2,
        labelY, knobWidth, 30);
}