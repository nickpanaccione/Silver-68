#pragma once

#include <JuceHeader.h>

namespace silver68 {

// toggle with a caption (bound to an apvts parameter)
class LabeledSwitch : public juce::Component {
public:
  LabeledSwitch (juce::AudioProcessorValueTreeState& state, juce::StringRef parameterID,
                 const juce::String& caption) {
    button.setButtonText (caption);
    addAndMakeVisible (button);

    attachment = std::make_unique<Attachment> (state, parameterID, button);
  }

  void resized() override { button.setBounds (getLocalBounds()); }

private:
  using Attachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

  juce::ToggleButton button;
  std::unique_ptr<Attachment> attachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledSwitch)
};

} // namespace silver68
