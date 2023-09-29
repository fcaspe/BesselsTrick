/*
 ==============================================================================
    Copyright (c) 2023 Franco Caspe
    All rights reserved.

    **BSD 3-Clause License**

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

 ==============================================================================

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
*/

/*
File: PluginProcessor.hpp
Plugin Processor Class (BesselsProcessor) declaration
*/

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_osc/juce_osc.h>

#include "BinaryData.h"
#include "Inference/EnvModels.hpp"
#include "FMSynth/FMSynth.hpp"
#include "FeatureProcessing/FeatureRegister.hpp"
#include "FeatureProcessing/RMSProcessor.hpp"
#include "FeatureProcessing/Yin.hpp"
#include "FeatureProcessing/PitchTrackManager.hpp"
#include "PluginConfig.hpp"
#include "ParameterIDs.hpp"


// Move to static method
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

//==============================================================================
class BesselsProcessor : public foleys::MagicProcessor,
                      private juce::AudioProcessorValueTreeState::Listener {
 public:
  //==============================================================================
  BesselsProcessor();
  ~BesselsProcessor() override;

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
  void sendDebugMessages(int fmblock, float pitch, float pitch_norm, float rms_in, std::vector<float> fm_ol);
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
  void updateMeters(float rms_in, float pitch,juce::AudioBuffer<float> &buffer);
  void updateGuiConfig();

  /* Model Routine Functions*/
  void load_gru_model(
      const std::string& model_filename);  // For standalone gru models
  void load_gru_model(const std::string& model_path, int n_state);
  void apply_config();
  void reload_model(const unsigned int entry);


  /* Application Specific attributes. */
  juce::AudioProcessorValueTreeState treeState;   // Plugin Tree State
  juce::AudioBuffer<float> _fm_render_buffer;     // Render buffer
  juce::AudioProcessLoadMeasurer _load_measurer;  // CPU Load measurer
  std::unique_ptr<FMSynth> _fmsynth;              // Synth.
  std::unique_ptr<EnvModel> _model;               // Resynthesis Model wrapper pointer.
  PitchTrackManager<4> _tracker_manager;          // Pitch tracker manager
  std::unique_ptr<RMS_Processor> _rms_processor;  // RMS Processor
  PluginConfig _config;                           // Config structure
  juce::OSCSender _osc_sender;                    // OSC IF
  FeatureRegister _feat_register;                 // Feature Register

 private:
  
  const int fm_block_size = 64;                    // Internal. RNN renders at 44.1kHz with block size of 64.
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
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BesselsProcessor)
};
