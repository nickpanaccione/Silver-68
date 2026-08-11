#include "PluginEditor.h"

namespace {

constexpr int margin = 12;
constexpr int headerHeight = 36;
constexpr int gateHeight = 120;
constexpr int titleHeight = 20;
constexpr int switchHeight = 24;
constexpr int knobWidth = 92;
constexpr int routingHeight = 24;

constexpr int editorWidth = 900;
constexpr int editorHeight = 460;

// splits row into equal columns (one per control)
void layoutRow (juce::Rectangle<int> area, const juce::Array<juce::Component*>& controls) {
  if (controls.isEmpty()) {
    return;
  }

  const auto width = area.getWidth() / controls.size();

  for (auto* control : controls) {
    control->setBounds (area.removeFromLeft (width).reduced (4, 0));
  }
}

} // namespace

Silver68Editor::Silver68Editor (Silver68Processor& p) :
    AudioProcessorEditor (p),
    state (p.apvts),
    bypass (state, "bypass", "Bypass"),
    input (state, "input", "Input"),
    output (state, "output", "Output"),
    bassChannelVolume (state, "bassChannelVolume", "Volume"),
    bassChannelTreble (state, "bassChannelTreble", "Treble"),
    bassChannelBass (state, "bassChannelBass", "Bass"),
    bassChannelDeep (state, "bassChannelDeep", "Deep"),
    normalChannelVolume (state, "normalChannelVolume", "Volume"),
    normalChannelTreble (state, "normalChannelTreble", "Treble"),
    normalChannelBass (state, "normalChannelBass", "Bass"),
    normalChannelBright (state, "normalChannelBright", "Bright"),
    gateOn (state, "gateOn", "Gate"),
    gateThreshold (state, "gateThreshold", "Threshold"),
    gateRelease (state, "gateRelease", "Release") {
  addSectionTitle (inputTitle, "INPUT");
  addSectionTitle (bassChannelTitle, "BASS CHANNEL");
  addSectionTitle (normalChannelTitle, "NORMAL CHANNEL");
  addSectionTitle (outputTitle, "OUTPUT");
  addSectionTitle (gateTitle, "GATE");

  if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter ("routing"))) {
    routing.addItemList (choice->choices, 1);
  }

  addAndMakeVisible (routing);
  routingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
      state, "routing", routing);

  const std::initializer_list<juce::Component*> controls{ &bypass,
                                                          &input,
                                                          &output,
                                                          &bassChannelVolume,
                                                          &bassChannelTreble,
                                                          &bassChannelBass,
                                                          &bassChannelDeep,
                                                          &normalChannelVolume,
                                                          &normalChannelTreble,
                                                          &normalChannelBass,
                                                          &normalChannelBright,
                                                          &gateOn,
                                                          &gateThreshold,
                                                          &gateRelease };

  for (auto* control : controls) {
    addAndMakeVisible (control);
  }

  setSize (editorWidth, editorHeight);
  setResizable (true, true);
  setResizeLimits (editorWidth / 2, editorHeight / 2, editorWidth * 2, editorHeight * 2);
}

void Silver68Editor::addSectionTitle (juce::Label& label, const juce::String& text) {
  label.setText (text, juce::dontSendNotification);
  label.setJustificationType (juce::Justification::centred);
  label.setFont (juce::FontOptions (13.0f, juce::Font::bold));
  addAndMakeVisible (label);
}

void Silver68Editor::paint (juce::Graphics& g) {
  g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

  g.setColour (juce::Colours::white.withAlpha (0.12f));
  auto area = getLocalBounds().reduced (margin);
  g.drawHorizontalLine (area.getY() + headerHeight, static_cast<float> (area.getX()),
                        static_cast<float> (area.getRight()));
  g.drawHorizontalLine (area.getBottom() - gateHeight, static_cast<float> (area.getX()),
                        static_cast<float> (area.getRight()));
}

void Silver68Editor::resized() {
  auto area = getLocalBounds().reduced (margin);

  layoutHeader (area.removeFromTop (headerHeight));
  layoutGate (area.removeFromBottom (gateHeight));
  layoutChannels (area);
}

void Silver68Editor::layoutHeader (juce::Rectangle<int> area) {
  bypass.setBounds (
      area.removeFromRight (knobWidth).withSizeKeepingCentre (knobWidth, switchHeight));
}

void Silver68Editor::layoutChannels (juce::Rectangle<int> area) {
  area.reduce (0, margin);

  // input and output flank the two channel sections
  auto inputArea = area.removeFromLeft (knobWidth + margin);
  auto outputArea = area.removeFromRight (knobWidth + margin);
  auto bassArea = area.removeFromLeft (area.getWidth() / 2);
  auto normalArea = area;

  inputTitle.setBounds (inputArea.removeFromTop (titleHeight));
  routing.setBounds (inputArea.removeFromBottom (routingHeight).reduced (4, 0));
  input.setBounds (inputArea.reduced (4, 0));

  outputTitle.setBounds (outputArea.removeFromTop (titleHeight));
  outputArea.removeFromBottom (routingHeight);
  output.setBounds (outputArea.reduced (4, 0));

  bassChannelTitle.setBounds (bassArea.removeFromTop (titleHeight));
  bassChannelDeep.setBounds (
      bassArea.removeFromBottom (switchHeight).withSizeKeepingCentre (knobWidth, switchHeight));
  layoutRow (bassArea, { &bassChannelVolume, &bassChannelTreble, &bassChannelBass });

  normalChannelTitle.setBounds (normalArea.removeFromTop (titleHeight));
  normalChannelBright.setBounds (
      normalArea.removeFromBottom (switchHeight).withSizeKeepingCentre (knobWidth, switchHeight));
  layoutRow (normalArea, { &normalChannelVolume, &normalChannelTreble, &normalChannelBass });
}

void Silver68Editor::layoutGate (juce::Rectangle<int> area) {
  area.reduce (0, margin / 2);
  gateTitle.setBounds (area.removeFromTop (titleHeight));

  auto controls = area.withSizeKeepingCentre (knobWidth * 3, area.getHeight());
  gateOn.setBounds (
      controls.removeFromLeft (knobWidth).withSizeKeepingCentre (knobWidth, switchHeight));
  layoutRow (controls, { &gateThreshold, &gateRelease });
}
