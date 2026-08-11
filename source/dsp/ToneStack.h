#pragma once

#include <JuceHeader.h>

namespace silver68 {

// passive tone stack (bass / treble / deep switch)
//
// passthrough stub
class ToneStack {
public:
  void prepare (const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    // TODO: compute initial coefficients
  }

  // pot rotations in [0, 1] (component-value mapping happens here)
  void setControls (float bass, float treble, bool deep) {
    // TODO: recompute coefficients on change, smoothed to avoid zipper noise
    juce::ignoreUnused (bass, treble, deep);
  }

  template <typename Context>
  void process (const Context& context) {
    // TODO: apply discretized tone stack filter
    juce::ignoreUnused (context);
  }

  void reset() {}

private:
  double sampleRate = 44100.0;
};

} // namespace silver68
