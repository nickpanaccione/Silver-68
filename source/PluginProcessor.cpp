#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {

constexpr int paramVersion = 1;

constexpr float gainSmoothingSeconds = 0.02f;

// front-panel dial (0-10) 
// rotation stays linear here (pot taper belongs to component)
std::unique_ptr<juce::AudioParameterFloat> makeDial (juce::StringRef id, juce::StringRef name,
                                                     float defaultValue) {
  return std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID{ id, paramVersion }, name,
      juce::NormalisableRange<float>{ 0.0f, 10.0f, 0.01f }, defaultValue,
      juce::AudioParameterFloatAttributes{}.withStringFromValueFunction (
          [] (float value, int) { return juce::String (value, 1); }));
}

std::unique_ptr<juce::AudioParameterFloat> makeDecibels (juce::StringRef id, juce::StringRef name,
                                                         float minDb, float maxDb,
                                                         float defaultDb) {
  return std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID{ id, paramVersion }, name,
      juce::NormalisableRange<float>{ minDb, maxDb, 0.1f }, defaultDb,
      juce::AudioParameterFloatAttributes{}.withLabel ("dB").withStringFromValueFunction (
          [] (float value, int) { return juce::String (value, 1); }));
}

std::unique_ptr<juce::AudioParameterBool> makeSwitch (juce::StringRef id, juce::StringRef name,
                                                      bool defaultValue) {
  return std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ id, paramVersion }, name,
                                                     defaultValue);
}

// output dial mapped to gain (0 is silence)
float dialToGain (float dial) {
  if (dial <= 0.0f) {
    return 0.0f;
  }

  return juce::Decibels::decibelsToGain (juce::jmap (dial, 0.0f, 10.0f, -40.0f, 0.0f));
}

} // namespace

Silver68Processor::Silver68Processor() :
    AudioProcessor (BusesProperties()
                        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    apvts (*this, nullptr, "PARAMS", createParameterLayout()) {
  inputParam = apvts.getRawParameterValue ("input");
  outputParam = apvts.getRawParameterValue ("output");
  bassChannelBassParam = apvts.getRawParameterValue ("bassChannelBass");
  bassChannelTrebleParam = apvts.getRawParameterValue ("bassChannelTreble");
  bassChannelDeepParam = apvts.getRawParameterValue ("bassChannelDeep");
  bypassParam = apvts.getParameter ("bypass");
}

juce::AudioProcessorValueTreeState::ParameterLayout Silver68Processor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add (makeSwitch ("bypass", "Bypass", false));

  // channel the guitar feeds ("Jumpered" drives both and sums them)
  layout.add (std::make_unique<juce::AudioParameterChoice> (
      juce::ParameterID{ "routing", paramVersion }, "Input Routing",
      juce::StringArray{ "Bass", "Normal", "Jumpered" }, 0));

  layout.add (makeDecibels ("input", "Input", -14.0f, 14.0f, 0.0f));
  layout.add (makeDial ("output", "Output", 5.0f));

  layout.add (makeDial ("bassChannelVolume", "Bass Ch Volume", 5.0f));
  layout.add (makeDial ("bassChannelTreble", "Bass Ch Treble", 5.0f));
  layout.add (makeDial ("bassChannelBass", "Bass Ch Bass", 5.0f));
  layout.add (makeSwitch ("bassChannelDeep", "Bass Ch Deep", false));

  layout.add (makeDial ("normalChannelVolume", "Normal Ch Volume", 5.0f));
  layout.add (makeDial ("normalChannelTreble", "Normal Ch Treble", 5.0f));
  layout.add (makeDial ("normalChannelBass", "Normal Ch Bass", 5.0f));
  layout.add (makeSwitch ("normalChannelBright", "Normal Ch Bright", false));

  layout.add (makeSwitch ("gateOn", "Gate", false));
  layout.add (makeDecibels ("gateThreshold", "Gate Threshold", -80.0f, 0.0f, -60.0f));

  auto releaseRange = juce::NormalisableRange<float>{ 5.0f, 500.0f };
  releaseRange.setSkewForCentre (100.0f);
  layout.add (std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID{ "gateRelease", paramVersion }, "Gate Release", releaseRange, 100.0f,
      juce::AudioParameterFloatAttributes{}.withLabel ("ms").withStringFromValueFunction (
          [] (float value, int) { return juce::String (juce::roundToInt (value)); })));

  return layout;
}

void Silver68Processor::prepareToPlay (double sampleRate, int samplesPerBlock) {
  juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock),
                               static_cast<juce::uint32> (getTotalNumOutputChannels()) };
  bassChannelTone.prepare (spec);

  inputGain.reset (sampleRate, gainSmoothingSeconds);
  outputGain.reset (sampleRate, gainSmoothingSeconds);
  inputGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (inputParam->load()));
  outputGain.setCurrentAndTargetValue (dialToGain (outputParam->load()));
}

bool Silver68Processor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  // mono or stereo, in == out
  const auto& out = layouts.getMainOutputChannelSet();

  if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo()) {
    return false;
  }

  return out == layouts.getMainInputChannelSet();
}

void Silver68Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
  juce::ScopedNoDenormals noDenormals;

  for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch) {
    buffer.clear (ch, 0, buffer.getNumSamples());
  }

  if (bypassParam != nullptr && bypassParam->getValue() >= 0.5f) {
    return;
  }

  inputGain.setTargetValue (juce::Decibels::decibelsToGain (inputParam->load()));
  inputGain.applyGain (buffer, buffer.getNumSamples());

  bassChannelTone.setControls (bassChannelBassParam->load(), bassChannelTrebleParam->load(),
                               bassChannelDeepParam->load() >= 0.5f);

  juce::dsp::AudioBlock<float> block (buffer);
  juce::dsp::ProcessContextReplacing<float> ctx (block);
  bassChannelTone.process (ctx);

  outputGain.setTargetValue (dialToGain (outputParam->load()));
  outputGain.applyGain (buffer, buffer.getNumSamples());
}

void Silver68Processor::getStateInformation (juce::MemoryBlock& destData) {
  auto state = apvts.copyState();
  state.setProperty ("schemaVersion", paramVersion, nullptr);

  if (auto xml = state.createXml()) {
    copyXmlToBinary (*xml, destData);
  }
}

void Silver68Processor::setStateInformation (const void* data, int sizeInBytes) {
  if (auto xml = getXmlFromBinary (data, sizeInBytes)) {
    apvts.replaceState (juce::ValueTree::fromXml (*xml));
  }
}

juce::AudioProcessorEditor* Silver68Processor::createEditor() {
  return new Silver68Editor (*this);
}

// JUCE plugin entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new Silver68Processor();
}
