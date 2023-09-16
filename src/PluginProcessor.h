#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_osc/juce_osc.h>

#include "BinaryData.h"
#include "DDSPModels.hpp"
#include "FMSynth.hpp"
#include "FeatureRegister.hpp"
#include "RMSProcessor.hpp"
#include "Yin.hpp"

// Move to static method
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

//==============================================================================
class FMTTProcessor : public foleys::MagicProcessor,
                      private juce::AudioProcessorValueTreeState::Listener {
 public:
  //==============================================================================
  FMTTProcessor();
  ~FMTTProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void initialiseBuilder(foleys::MagicGUIBuilder& builder) override;
  void parameterChanged(const juce::String& param, float value) override;
  void postSetStateInformation () override;
  void configure_gui_listeners();
  void add_triggers();
  void showLoadDialog();
  void loadModelList();
  void setupMeters();
  void setupValueTree();
  void updateKnob(juce::String knob_id, double value);
  void updateKnobs();
  void updateAlgoPlot(const int algo_num);

  /* Model Routine Functions*/
  void load_gru_model(
      const std::string& model_filename);  // For standalone gru models
  void load_gru_model(const std::string& model_path, int n_state);
  void apply_config();
  void reload_model(const unsigned int entry);
  // void handle_frs(int entry_id);
  /* Application Specific attributes. */
  juce::AudioProcessorValueTreeState treeState;
  // float rms_norm_feedback;
  std::vector<float> _feedbackBuffer;
  juce::AudioProcessLoadMeasurer _load_measurer;  // Load measurer
  std::unique_ptr<FMSynth> _fmsynth;              // Synth.
  std::unique_ptr<DDSPModel> _model;  // Resynthesis Model wrapper pointer.
  std::unique_ptr<Yin> _pitch_tracker;
  std::unique_ptr<RMS_Processor> _rms_processor;
  // std::unique_ptr<RMS_Processor> _rms_processor_feedback;
  PluginConfig _config;
  PluginEditorConfig _guiconfig;
  juce::OSCSender _osc_sender;
  FeatureRegister _feat_register;

 private:
  juce::AudioFormatManager manager;

  // Workaround to fetch MagicGUIBuilder from MagicPlugin class without
  // overriding createEditor() (which in practice should not be declared by
  // plugin)
  foleys::MagicGUIBuilder* builder_ptr = 0;
  foleys::MagicLevelSource* outputLevel{nullptr};
  juce::LookAndFeel_V1 plotLookAndFeel;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FMTTProcessor)
};
