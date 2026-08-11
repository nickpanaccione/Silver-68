#pragma once

#include <JuceHeader.h>
#include "dsp/ToneStack.h"

class Silver68Processor : public juce::AudioProcessor {
public:
  Silver68Processor();
  ~Silver68Processor() override = default;

  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  juce::AudioProcessorParameter* getBypassParameter() const override { return bypassParam; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram (int) override {}
  const juce::String getProgramName (int) override { return {}; }
  void changeProgramName (int, const juce::String&) override {}

  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState apvts;

private:
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // cached at construction (getRawParameterValue does a string lookup)
  std::atomic<float>* inputParam = nullptr;
  std::atomic<float>* outputParam = nullptr;
  std::atomic<float>* bassChannelBassParam = nullptr;
  std::atomic<float>* bassChannelTrebleParam = nullptr;
  std::atomic<float>* bassChannelDeepParam = nullptr;
  juce::RangedAudioParameter* bypassParam = nullptr;

  juce::SmoothedValue<float> inputGain;
  juce::SmoothedValue<float> outputGain;

  // stages in signal-chain order
  silver68::ToneStack bassChannelTone;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Silver68Processor)
};
