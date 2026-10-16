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
    const int centerX = getWidth() / 2;

    driveSlider.setBounds(centerX - 65, 105, 130, 145);
    driveLabel.setBounds(centerX - 65, 250, 130, 30);
}