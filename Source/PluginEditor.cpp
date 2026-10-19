#include "PluginEditor.h"

VocalSaturatorAudioProcessorEditor::
VocalSaturatorAudioProcessorEditor(
    VocalSaturatorAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p)
{
    auto setupSlider = [this](juce::Slider& slider)
        {
            slider.setSliderStyle(
                juce::Slider::RotaryHorizontalVerticalDrag);

            slider.setTextBoxStyle(
                juce::Slider::TextBoxBelow,
                false, 90, 24);

            addAndMakeVisible(slider);
        };

    auto setupLabel = [this](
        juce::Label& label,
        const juce::String& text)
        {
            label.setText(text, juce::dontSendNotification);

            label.setJustificationType(
                juce::Justification::centred);

            label.setColour(
                juce::Label::textColourId,
                juce::Colours::white);

            addAndMakeVisible(label);
        };

    setupSlider(driveSlider);
    setupSlider(mixSlider);
    setupSlider(toneSlider);
    setupSlider(outputSlider);

    setupLabel(driveLabel, "DRIVE");
    setupLabel(mixLabel, "MIX");
    setupLabel(toneLabel, "TONE");
    setupLabel(outputLabel, "OUTPUT");

    driveSlider.setTextValueSuffix(" dB");
    mixSlider.setTextValueSuffix(" %");
    toneSlider.setTextValueSuffix(" dB");
    outputSlider.setTextValueSuffix(" dB");

    driveAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, "drive", driveSlider);

    mixAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, "mix", mixSlider);

    toneAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.parameters, "tone", toneSlider);

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
    constexpr int knobWidth = 110;
    constexpr int knobHeight = 145;
    constexpr int spacing = 12;

    const int totalWidth = knobWidth * 4 + spacing * 3;
    const int startX = (getWidth() - totalWidth) / 2;

    constexpr int knobY = 105;
    constexpr int labelY = 250;

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

    toneSlider.setBounds(
        startX + (knobWidth + spacing) * 2,
        knobY, knobWidth, knobHeight);

    toneLabel.setBounds(
        startX + (knobWidth + spacing) * 2,
        labelY, knobWidth, 30);

    outputSlider.setBounds(
        startX + (knobWidth + spacing) * 3,
        knobY, knobWidth, knobHeight);

    outputLabel.setBounds(
        startX + (knobWidth + spacing) * 3,
        labelY, knobWidth, 30);
}