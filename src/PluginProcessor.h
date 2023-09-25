#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_osc/juce_osc.h>

#include "BinaryData.h"
#include "EnvModels.hpp"
#include "FMSynth.hpp"
#include "FeatureRegister.hpp"
#include "RMSProcessor.hpp"
#include "Yin.hpp"
#include "PitchTrackManager.hpp"


namespace ValTree_IDs {
  static juce::Identifier gui_params ("gui_params"); // pre-create an Identifier
}

// ID's for objects in GUI
namespace GUI_IDs {
static juce::String status{"lbl_status"};
static juce::String models{"combobox_models"};
static juce::String algorithm{"knob_algo"};
static juce::String algoplot{"label_algo"};
static juce::String inGain{"knob_input_gain"};
static juce::String outGain{"knob_output_gain"};

static juce::String fmBoost1{"knob_boost1"};
static juce::String fmBoost2{"knob_boost2"};
static juce::String fmBoost3{"knob_boost3"};
static juce::String fmBoost4{"knob_boost4"};
static juce::String fmBoost5{"knob_boost5"};
static juce::String fmBoost6{"knob_boost6"};

static juce::String fmFine1{"knob_fine1"};
static juce::String fmFine2{"knob_fine2"};
static juce::String fmFine3{"knob_fine3"};
static juce::String fmFine4{"knob_fine4"};
static juce::String fmFine5{"knob_fine5"};
static juce::String fmFine6{"knob_fine6"};

static juce::String fmCoarse1{"knob_coarse1"};
static juce::String fmCoarse2{"knob_coarse2"};
static juce::String fmCoarse3{"knob_coarse3"};
static juce::String fmCoarse4{"knob_coarse4"};
static juce::String fmCoarse5{"knob_coarse5"};
static juce::String fmCoarse6{"knob_coarse6"};

}  // namespace GUI_IDs

// IDs for properties in ValueTree
namespace IDs {
static juce::String cboxselectedid{"cboxselectedid"};
static juce::String inGain{"in_gain"};
static juce::String outGain{"out_gain"};
static juce::String algorithm{"algorithm"};

static juce::String fmBoost1{"boost1"};
static juce::String fmBoost2{"boost2"};
static juce::String fmBoost3{"boost3"};
static juce::String fmBoost4{"boost4"};
static juce::String fmBoost5{"boost5"};
static juce::String fmBoost6{"boost6"};

static juce::String fmFine1{"fine1"};
static juce::String fmFine2{"fine2"};
static juce::String fmFine3{"fine3"};
static juce::String fmFine4{"fine4"};
static juce::String fmFine5{"fine5"};
static juce::String fmFine6{"fine6"};

static juce::String fmCoarse1{"coarse1"};
static juce::String fmCoarse2{"coarse2"};
static juce::String fmCoarse3{"coarse3"};
static juce::String fmCoarse4{"coarse4"};
static juce::String fmCoarse5{"coarse5"};
static juce::String fmCoarse6{"coarse6"};

static juce::String debug1{"debug1"};
static juce::String debug2{"debug2"};
static juce::String debug3{"debug3"};
static juce::String debug5{"debug5"};
static juce::String debug6{"debug6"};
static juce::String debug7{"debug7"};
static juce::String debug8{"debug8"};
static juce::String debug9{"debug9"};

static juce::Identifier oscilloscope{"oscilloscope"};
}  // namespace IDs

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
  void storeToValTree(juce::Identifier child_id,juce::String property,juce::var value);
  void configure_gui_listeners();
  void add_triggers();
  void printValueTree();
  void showLoadDialog();
  void loadModelList();
  void setupMeters();
  void setupValueTree();
  void updateKnob(juce::String knob_id, double value);
  void updateKnobs();
  void updateGuiConfig();

  /* Model Routine Functions*/
  void load_gru_model(
      const std::string& model_filename);  // For standalone gru models
  void load_gru_model(const std::string& model_path, int n_state);
  void apply_config();
  void reload_model(const unsigned int entry);


  /* Application Specific attributes. */
  juce::AudioProcessorValueTreeState treeState;
  //std::vector<float> _feedbackBuffer;
  juce::AudioProcessLoadMeasurer _load_measurer;  // Load measurer
  std::unique_ptr<FMSynth> _fmsynth;              // Synth.
  std::unique_ptr<EnvModel> _model;  // Resynthesis Model wrapper pointer.
  //std::unique_ptr<Yin> _pitch_tracker;
  PitchTrackManager<4> _tracker_manager;
  std::unique_ptr<RMS_Processor> _rms_processor;
  // std::unique_ptr<RMS_Processor> _rms_processor_feedback;
  PluginConfig _config;
  juce::OSCSender _osc_sender;
  FeatureRegister _feat_register;

 private:
  juce::AudioFormatManager manager;

  // Workaround to fetch MagicGUIBuilder from MagicPlugin class without
  // overriding createEditor() (which in practice should not be declared by
  // plugin)
  foleys::MagicGUIBuilder* builder_ptr = 0;
  foleys::MagicLevelSource* _input_rms_meter{nullptr};
  foleys::MagicLevelSource* _input_f0_meter{nullptr};
  foleys::MagicLevelSource* _output_meter{nullptr};
  juce::LookAndFeel_V1 plotLookAndFeel;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FMTTProcessor)
};
