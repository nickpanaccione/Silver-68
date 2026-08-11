#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "gui/LabeledKnob.h"
#include "gui/LabeledSwitch.h"

class Silver68Editor : public juce::AudioProcessorEditor {
public:
  explicit Silver68Editor (Silver68Processor&);

  void paint (juce::Graphics&) override;
  void resized() override;

private:
  void addSectionTitle (juce::Label&, const juce::String& text);
  void layoutHeader (juce::Rectangle<int> area);
  void layoutChannels (juce::Rectangle<int> area);
  void layoutGate (juce::Rectangle<int> area);

  juce::AudioProcessorValueTreeState& state;

  juce::Label inputTitle, bassChannelTitle, normalChannelTitle, outputTitle, gateTitle;

  silver68::LabeledSwitch bypass;
  juce::ComboBox routing;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> routingAttachment;

  silver68::LabeledKnob input, output;
  silver68::LabeledKnob bassChannelVolume, bassChannelTreble, bassChannelBass;
  silver68::LabeledSwitch bassChannelDeep;
  silver68::LabeledKnob normalChannelVolume, normalChannelTreble, normalChannelBass;
  silver68::LabeledSwitch normalChannelBright;
  silver68::LabeledSwitch gateOn;
  silver68::LabeledKnob gateThreshold, gateRelease;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Silver68Editor)
};
