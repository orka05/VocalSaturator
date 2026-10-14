#include "PluginEditor.h"

VocalSaturatorAudioProcessorEditor::
VocalSaturatorAudioProcessorEditor(
    VocalSaturatorAudioProcessor& processor)
    : juce::AudioProcessorEditor(&processor),
    audioProcessor(processor)
{
    setSize(520, 320);
}

void VocalSaturatorAudioProcessorEditor::paint(
    juce::Graphics& graphics)
{
    graphics.fillAll(juce::Colour::fromRGB(24, 26, 31));

    graphics.setColour(juce::Colours::white);
    graphics.setFont(juce::Font(26.0f));

    graphics.drawFittedText(
        "VOCAL SATURATOR",
        getLocalBounds().removeFromTop(110),
        juce::Justification::centred,
        1
    );

    graphics.setColour(juce::Colour::fromRGB(160, 165, 175));
    graphics.setFont(15.0f);

    graphics.drawFittedText(
        "Audio engine initialized",
        getLocalBounds().reduced(20),
        juce::Justification::centred,
        1
    );
}

void VocalSaturatorAudioProcessorEditor::resized()
{
    // GUI controls will be added here later.
}
