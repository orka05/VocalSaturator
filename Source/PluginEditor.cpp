#include "PluginEditor.h"

#include <cmath>

namespace
{
    // ========================================================
    // Colours
    // ========================================================

    const juce::Colour backgroundColour =
        juce::Colour::fromRGB(22, 29, 40);

    const juce::Colour topBackgroundColour =
        juce::Colour::fromRGB(35, 49, 68);

    const juce::Colour knobColour =
        juce::Colour::fromRGB(40, 53, 73);

    const juce::Colour trackColour =
        juce::Colour::fromRGB(62, 80, 105);

    const juce::Colour accentColour =
        juce::Colour::fromRGB(100, 174, 255);

    const juce::Colour primaryTextColour =
        juce::Colour::fromRGB(248, 250, 255);

    const juce::Colour secondaryTextColour =
        juce::Colour::fromRGB(141, 159, 184);

    const juce::Colour negativeToneColour =
        juce::Colour::fromRGB(5, 8, 14);

    // ========================================================
    // Constants
    // ========================================================

    constexpr float rotaryStart =
        juce::MathConstants<float>::pi * 1.25f;

    constexpr float rotaryEnd =
        juce::MathConstants<float>::pi * 2.75f;

    constexpr int pluginWidth = 720;
    constexpr int pluginHeight = 250;

    constexpr int knobSize = 132;
    constexpr int labelHeight = 25;

    // ========================================================
    // Font helper
    // ========================================================

    juce::Font makeFont(float size, bool bold = false)
    {
        auto options = juce::FontOptions(size)
            .withName("Segoe UI");

        if (bold)
            options = options.withStyle("Bold");

        return juce::Font(options);
    }

    // ========================================================
    // Fixing tab shortcut in DAW
    // ========================================================

    void disableKeyboardFocus(juce::Component& component)
    {
        component.setWantsKeyboardFocus(false);
        component.setMouseClickGrabsKeyboardFocus(false);

        for (auto* child : component.getChildren())
            disableKeyboardFocus(*child);
    }
}

// ============================================================
// LookAndFeel constructor
// ============================================================

VocalSaturatorLookAndFeel::VocalSaturatorLookAndFeel()
{
    setColour(
        juce::Slider::textBoxTextColourId,
        primaryTextColour);

    setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colours::transparentBlack);

    setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colours::transparentBlack);

    setColour(
        juce::Slider::textBoxHighlightColourId,
        accentColour.withAlpha(0.35f));
}

// ============================================================
// Custom slider text box
// ============================================================

juce::Label* VocalSaturatorLookAndFeel::createSliderTextBox(
    juce::Slider& slider)
{
    auto* label =
        juce::LookAndFeel_V4::createSliderTextBox(slider);

    label->setFont(makeFont(27.0f, true));

    label->setJustificationType(
        juce::Justification::centred);

    label->setColour(
        juce::Label::textColourId,
        primaryTextColour);

    label->setColour(
        juce::Label::backgroundColourId,
        juce::Colours::transparentBlack);

    label->setColour(
        juce::Label::outlineColourId,
        juce::Colours::transparentBlack);

    label->setColour(
        juce::Label::textWhenEditingColourId,
        primaryTextColour);

    label->setColour(
        juce::Label::backgroundWhenEditingColourId,
        knobColour);

    label->setColour(
        juce::Label::outlineWhenEditingColourId,
        accentColour);

    label->setWantsKeyboardFocus(false);
    label->setMouseClickGrabsKeyboardFocus(false);

    return label;
}

// ============================================================
// Custom slider layout
// ============================================================

juce::Slider::SliderLayout
VocalSaturatorLookAndFeel::getSliderLayout(
    juce::Slider& slider)
{
    juce::Slider::SliderLayout layout;

    const auto bounds = slider.getLocalBounds();

    // Entire component is used for the rotary knob.
    layout.sliderBounds = bounds;

    // Make only the actual number clickable, leaving the knob draggable.
    juce::GlyphArrangement glyphs;
    glyphs.addLineOfText(makeFont(27.0f, true),
        slider.getTextFromValue(slider.getValue()),
        0.0f, 0.0f);

    const int textWidth = juce::jlimit(
        24, bounds.getWidth() - 16,
        juce::roundToInt(glyphs.getBoundingBox(0, -1, true).getWidth()) + 6);

    layout.textBoxBounds = juce::Rectangle<int>(
        bounds.getCentreX() - textWidth / 2,
        bounds.getCentreY() - 18,
        textWidth,
        36);

    return layout;
}

// ============================================================
// Rotary knob drawing
// ============================================================

void VocalSaturatorLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float startAngle,
    float endAngle,
    juce::Slider& slider)
{
    const float cx =
        static_cast<float>(x) +
        static_cast<float>(width) * 0.5f;

    const float cy =
        static_cast<float>(y) +
        static_cast<float>(height) * 0.5f;

    const float diameter =
        static_cast<float>(juce::jmin(width, height)) - 16.0f;

    const float radius = diameter * 0.5f;
    const float ringRadius = radius - 2.0f;

    const float angle =
        startAngle +
        sliderPos * (endAngle - startAngle);

    const bool isTone =
        slider.getName() == "tone";

    const float neutralPosition = isTone
        ? static_cast<float>(
            slider.valueToProportionOfLength(0.0))
        : 0.5f;

    const float neutralAngle =
        startAngle +
        neutralPosition * (endAngle - startAngle);

    // ========================================================
    // Main knob body
    // ========================================================

    g.setColour(knobColour);

    g.fillEllipse(
        cx - radius,
        cy - radius,
        diameter,
        diameter);

    // ========================================================
    // Background ring
    // ========================================================

    juce::Path backgroundArc;

    backgroundArc.addCentredArc(
        cx,
        cy,
        ringRadius,
        ringRadius,
        0.0f,
        startAngle,
        endAngle,
        true);

    g.setColour(trackColour);

    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(
            4.0f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    // ========================================================
    // Active ring
    // ========================================================

    if (isTone)
    {
        if (sliderPos < neutralPosition - 0.001f)
        {
            juce::Path negativeArc;

            negativeArc.addCentredArc(
                cx,
                cy,
                ringRadius,
                ringRadius,
                0.0f,
                angle,
                neutralAngle,
                true);

            g.setColour(negativeToneColour);

            g.strokePath(
                negativeArc,
                juce::PathStrokeType(
                    4.5f,
                    juce::PathStrokeType::curved,
                    juce::PathStrokeType::rounded));
        }
        else if (sliderPos > neutralPosition + 0.001f)
        {
            juce::Path positiveArc;

            positiveArc.addCentredArc(
                cx,
                cy,
                ringRadius,
                ringRadius,
                0.0f,
                neutralAngle,
                angle,
                true);

            g.setColour(juce::Colours::white);

            g.strokePath(
                positiveArc,
                juce::PathStrokeType(
                    4.5f,
                    juce::PathStrokeType::curved,
                    juce::PathStrokeType::rounded));
        }
    }
    else if (sliderPos > 0.001f)
    {
        juce::Path activeArc;

        activeArc.addCentredArc(
            cx,
            cy,
            ringRadius,
            ringRadius,
            0.0f,
            startAngle,
            angle,
            true);

        g.setColour(accentColour);

        g.strokePath(
            activeArc,
            juce::PathStrokeType(
                4.5f,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded));
    }

    // ========================================================
    // Position indicator - small dot, no needle
    // ========================================================

    const float indicatorRadius = radius * 0.73f;

    const float dotX =
        cx + std::sin(angle) * indicatorRadius;

    const float dotY =
        cy - std::cos(angle) * indicatorRadius;

    if (isTone)
    {
        if (sliderPos < neutralPosition - 0.001f)
            g.setColour(negativeToneColour);
        else if (sliderPos > neutralPosition + 0.001f)
            g.setColour(juce::Colours::white);
        else
            g.setColour(secondaryTextColour);
    }
    else
    {
        g.setColour(accentColour);
    }

    g.fillEllipse(
        dotX - 3.0f,
        dotY - 3.0f,
        6.0f,
        6.0f);
}

// ============================================================
// Editor constructor
// ============================================================

VocalSaturatorAudioProcessorEditor::
VocalSaturatorAudioProcessorEditor(
    VocalSaturatorAudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
    audioProcessor(p),
    tooltipWindow(this, 2000)
{
    tooltipWindow.setMillisecondsBeforeTipAppears(2000);

    setWantsKeyboardFocus(false);
    setMouseClickGrabsKeyboardFocus(false);

    // ========================================================
    // Header
    // ========================================================

    titleLabel.setText(
        "VOCAL SATURATOR",
        juce::dontSendNotification);

    titleLabel.setFont(makeFont(33.0f, true));

    titleLabel.setJustificationType(
        juce::Justification::centredLeft);

    titleLabel.setColour(
        juce::Label::textColourId,
        primaryTextColour);

    addAndMakeVisible(titleLabel);

    subtitleLabel.setText(
        "by orka",
        juce::dontSendNotification);

    subtitleLabel.setFont(makeFont(15.0f));

    subtitleLabel.setJustificationType(
        juce::Justification::centredLeft);

    subtitleLabel.setColour(
        juce::Label::textColourId,
        accentColour);

    addAndMakeVisible(subtitleLabel);

    // ========================================================
    // Tooltip descriptions
    // ========================================================

    const juce::String driveTip =
        "DRIVE\n\n"
        "Controls how hard the vocal is driven into "
        "the saturation stage.\n\n"
        "Higher values add harmonics, density and "
        "a more aggressive character.";

    const juce::String mixTip =
        "MIX\n\n"
        "Blends the saturated signal with the original "
        "dry signal.\n\n"
        "100% means fully processed. Lower values "
        "preserve more of the original vocal.";

    const juce::String toneTip =
        "TONE\n\n"
        "Shapes the tonal balance of the saturated signal.\n\n"
        "Turn right for a brighter sound and left for "
        "a darker sound. At 0 dB, Tone is neutral.";

    const juce::String outputTip =
        "OUTPUT\n\n"
        "Controls the final output level after processing.\n\n"
        "Use it to compensate for loudness changes "
        "caused by saturation.";

    // ========================================================
    // Slider setup
    // ========================================================

    auto setupSlider = [this](
        juce::Slider& slider,
        const juce::String& name)
        {
            slider.setName(name);

            slider.setSliderStyle(
                juce::Slider::RotaryHorizontalVerticalDrag);

            slider.setRotaryParameters(
                rotaryStart,
                rotaryEnd,
                true);

            slider.setTextBoxStyle(
                juce::Slider::TextBoxBelow,
                false,
                60,
                36);

            slider.onValueChange = [&slider]() { slider.resized(); };
            slider.setMouseDragSensitivity(180);
            slider.setLookAndFeel(&customLookAndFeel);

            slider.setColour(
                juce::Slider::textBoxTextColourId,
                primaryTextColour);

            slider.setColour(
                juce::Slider::textBoxBackgroundColourId,
                juce::Colours::transparentBlack);

            slider.setColour(
                juce::Slider::textBoxOutlineColourId,
                juce::Colours::transparentBlack);

            // No tooltip on the knob or its numeric value.
            slider.setTooltip({});

            addAndMakeVisible(slider);

            slider.setWantsKeyboardFocus(false);
            slider.setMouseClickGrabsKeyboardFocus(false);
        };

    // ========================================================
    // Parameter label setup
    // ========================================================

    auto setupLabel = [this](
        juce::Label& label,
        const juce::String& text,
        const juce::String& tooltip)
        {
            label.setText(
                text,
                juce::dontSendNotification);

            label.setFont(makeFont(15.0f, true));

            label.setJustificationType(
                juce::Justification::centred);

            label.setColour(
                juce::Label::textColourId,
                primaryTextColour);

            label.setTooltip(tooltip);

            addAndMakeVisible(label);
        };

    // ========================================================
    // Unit label setup
    // ========================================================

    auto setupUnitLabel = [this](
        juce::Label& label,
        const juce::String& unit)
        {
            label.setText(
                unit,
                juce::dontSendNotification);

            label.setFont(makeFont(15.0f));

            label.setJustificationType(
                juce::Justification::centred);

            label.setColour(
                juce::Label::textColourId,
                secondaryTextColour);

            label.setTooltip({});
            label.setInterceptsMouseClicks(false, false);

            addAndMakeVisible(label);
        };

    // ========================================================
    // Create sliders and labels
    // ========================================================

    setupSlider(driveSlider, "drive");
    setupSlider(mixSlider, "mix");
    setupSlider(toneSlider, "tone");
    setupSlider(outputSlider, "output");

    setupLabel(driveLabel, "DRIVE", driveTip);
    setupLabel(mixLabel, "MIX", mixTip);
    setupLabel(toneLabel, "TONE", toneTip);
    setupLabel(outputLabel, "OUTPUT", outputTip);

    setupUnitLabel(driveUnitLabel, "dB");
    setupUnitLabel(mixUnitLabel, "%");
    setupUnitLabel(toneUnitLabel, "dB");
    setupUnitLabel(outputUnitLabel, "dB");

    // ========================================================
    // Numeric formatting
    // ========================================================

    driveSlider.setNumDecimalPlacesToDisplay(1);
    mixSlider.setNumDecimalPlacesToDisplay(0);
    toneSlider.setNumDecimalPlacesToDisplay(1);
    outputSlider.setNumDecimalPlacesToDisplay(1);

    // ========================================================
    // APVTS attachments
    // ========================================================

    driveAttachment =
        std::make_unique<SliderAttachment>(
            audioProcessor.parameters,
            "drive",
            driveSlider);

    mixAttachment =
        std::make_unique<SliderAttachment>(
            audioProcessor.parameters,
            "mix",
            mixSlider);

    toneAttachment =
        std::make_unique<SliderAttachment>(
            audioProcessor.parameters,
            "tone",
            toneSlider);

    outputAttachment =
        std::make_unique<SliderAttachment>(
            audioProcessor.parameters,
            "output",
            outputSlider);

    setSize(pluginWidth, pluginHeight);
    disableKeyboardFocus(*this);
}

// ============================================================
// Destructor
// ============================================================

VocalSaturatorAudioProcessorEditor::
~VocalSaturatorAudioProcessorEditor()
{
    driveSlider.setLookAndFeel(nullptr);
    mixSlider.setLookAndFeel(nullptr);
    toneSlider.setLookAndFeel(nullptr);
    outputSlider.setLookAndFeel(nullptr);
}

// ============================================================
// Background
// ============================================================

void VocalSaturatorAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    juce::ColourGradient background(
        topBackgroundColour,
        0.0f,
        0.0f,
        backgroundColour,
        0.0f,
        static_cast<float>(getHeight()),
        false);

    g.setGradientFill(background);
    g.fillAll();
}

// ============================================================
// Layout
// ============================================================


void VocalSaturatorAudioProcessorEditor::resized()
{
    const int w = getWidth();
    titleLabel.setBounds(22, 9, w - 44, 40);
    subtitleLabel.setBounds(24, 45, w - 48, 19);

    const int knobY = 70;
    const int labelY = knobY + knobSize + 5;

    auto placeControl = [this, w, knobY, labelY](
        int index,
        juce::Label& nameLabel,
        juce::Slider& slider,
        juce::Label& unitLabel)
        {
            const int centerX = (w * (2 * index + 1)) / 8;
            slider.setBounds(centerX - knobSize / 2, knobY, knobSize, knobSize);
            nameLabel.setBounds(centerX - 65, labelY, 130, labelHeight);
            unitLabel.setBounds(centerX - 30, knobY + 79, 60, 22);
            unitLabel.toFront(false);
        };

    placeControl(0, driveLabel, driveSlider, driveUnitLabel);
    placeControl(1, mixLabel, mixSlider, mixUnitLabel);
    placeControl(2, toneLabel, toneSlider, toneUnitLabel);
    placeControl(3, outputLabel, outputSlider, outputUnitLabel);
}