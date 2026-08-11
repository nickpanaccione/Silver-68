#pragma once

#include <JuceHeader.h>

namespace silver68 {

// rotary control with caption (bound to an apvts parameter)
class LabeledKnob : public juce::Component {
public:
  LabeledKnob (juce::AudioProcessorValueTreeState& state, juce::StringRef parameterID,
               const juce::String& caption) {
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    addAndMakeVisible (knob);

    label.setText (caption, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (label);

    attachment = std::make_unique<Attachment> (state, parameterID, knob);
  }

  void resized() override {
    auto area = getLocalBounds();
    label.setBounds (area.removeFromTop (labelHeight));
    knob.setBounds (area);
  }

private:
  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

  static constexpr int labelHeight = 16;
  static constexpr int textBoxWidth = 56;
  static constexpr int textBoxHeight = 16;

  juce::Slider knob;
  juce::Label label;
  std::unique_ptr<Attachment> attachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledKnob)
};

} // namespace silver68
