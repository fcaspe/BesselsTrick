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
File: PluginProcessorGUI.cpp
Implements GUI methods within foleys::MagicProcessor plugin class.
*/
#include <cmath>

#include "GuiItems.hpp"
#include "PluginProcessor.hpp"

/**
initialiseBuilder(): 
    Here we initialize the GUI Builder
*/
void FMTTProcessor::initialiseBuilder(foleys::MagicGUIBuilder& builder) {
  builder.registerJUCEFactories();
  builder.registerJUCELookAndFeels();
  
  builder.registerFactory("DrawableLabel", &DrawableLabelItem::factory);
  builder.registerFactory("ModelComboBox", &ModelComboBoxItem::factory);
  builder.registerFactory("StatusBar", &StatusBarItem::factory);
  builder.registerFactory("RatiosBar", &RatiosBarItem::factory);
  
  // Workaround to fetch builder from MagicPlugin without
  // overriding createEditor() (which should not be declared by plugin)
  builder_ptr = &builder;
}

void FMTTProcessor::setupMeters() {
  _input_rms_meter =
    magicState.createAndAddObject<foleys::MagicLevelSource>("input_rms");
  if(_input_rms_meter)
    _input_rms_meter->setupSource(1,getSampleRate()/getBlockSize(),100);
  _input_f0_meter =
    magicState.createAndAddObject<foleys::MagicLevelSource>("input_f0");
  if(_input_f0_meter)
    _input_f0_meter->setupSource(1,getSampleRate()/getBlockSize(),100);
  _output_meter =
    magicState.createAndAddObject<foleys::MagicLevelSource>("output_level");
  if(_output_meter)
    _output_meter->setupSource(1,getSampleRate(),100);
}

/**
updateKnob(): 
    When we load a new model, update knobs
*/
void FMTTProcessor::updateKnob(juce::String knob_id, double value) {
  // Find knob
  if (auto* item = builder_ptr->findGuiItemWithId(knob_id)) {
    // std::cout << "Found " << knob_id << std::endl;
    if (auto* slider =
            dynamic_cast<juce::Slider*>(item->getWrappedComponent())) {
      slider->setValue(value);
    }
  }
}

void FMTTProcessor::updateKnobs() {
  // Each knob update will trigger a notification which in turn,
  // Will update the internal plugin state
  updateKnob(GUI_IDs::algorithm, _config.fm_config + 1);
  
  updateKnob(GUI_IDs::fmCoarse1, _config.fm_coarse[0]);
  updateKnob(GUI_IDs::fmCoarse2, _config.fm_coarse[1]);
  updateKnob(GUI_IDs::fmCoarse3, _config.fm_coarse[2]);
  updateKnob(GUI_IDs::fmCoarse4, _config.fm_coarse[3]);
  updateKnob(GUI_IDs::fmCoarse5, _config.fm_coarse[4]);
  updateKnob(GUI_IDs::fmCoarse6, _config.fm_coarse[5]);

  updateKnob(GUI_IDs::fmFine1, _config.fm_fine[0]);
  updateKnob(GUI_IDs::fmFine2, _config.fm_fine[1]);
  updateKnob(GUI_IDs::fmFine3, _config.fm_fine[2]);
  updateKnob(GUI_IDs::fmFine4, _config.fm_fine[3]);
  updateKnob(GUI_IDs::fmFine5, _config.fm_fine[4]);
  updateKnob(GUI_IDs::fmFine6, _config.fm_fine[5]);
}

/**
updateAlgoPlot(): 
    When algorithm changes, update graphic
*/
void FMTTProcessor::updateGuiConfig() {
  auto *guiconfig = magicState.getObjectWithType<PluginGUIConfig>("guiconfig");
  if(guiconfig != nullptr)
  {
    guiconfig->fm_algo = _config.fm_config+1;
    guiconfig->fm_coarse = _config.fm_coarse;
    guiconfig->fm_fine = _config.fm_fine;
  }
}

void FMTTProcessor::setupValueTree()
{
  if(magicState.createAndAddObject<PluginGUIConfig>("guiconfig"));
  else
    std::cout << "[SETUP VAL TREE] Error creating Object: guiconfig";

}

void FMTTProcessor::storeToValTree(juce::Identifier child_id,
  juce::String property,juce::var value)
{
  auto &val_tree = magicState.getValueTree();
  auto child = val_tree.getChildWithName(child_id);
  if(child.isValid() == false)
  {
    juce::ValueTree newChild (child_id);
    newChild.setProperty(property,value,nullptr);
    val_tree.addChild(newChild,-1,nullptr);
    //if (myNode.hasProperty (nameProperty)) {}
  }
  else
  {
    child.setProperty(property,value,nullptr);
  }
}

void FMTTProcessor::updateMeters(float rms_in, float pitch, 
  juce::AudioBuffer<float> &buffer)
{
  if (_input_rms_meter)
    {
      auto input_buffer = juce::AudioBuffer<float>(1,1);
      //Denormalize rms before feeding into meter
      input_buffer.getWritePointer(0)[0] = powf(20,((rms_in-1.33f)*60.0f)/20.0f);
      _input_rms_meter->pushSamples(input_buffer);
    }
  if (_input_f0_meter)
    {
      auto input_buffer = juce::AudioBuffer<float>(1,1);
      // Scale pitch for feeding into meter
      input_buffer.getWritePointer(0)[0] = pitch > 3000? 1.0 : pitch/3000 ;
      _input_f0_meter->pushSamples(input_buffer);
    }
  if (_output_meter) _output_meter->pushSamples(buffer);

}

void FMTTProcessor::postSetStateInformation()
{
  std::cout << "[postSetStateInformation] Recalling from treeState" << std::endl;
  auto *guiconfig = magicState.getObjectWithType<PluginGUIConfig>("guiconfig");
  if(guiconfig)
  {
  auto &val_tree = magicState.getValueTree();
  guiconfig->modeldir = val_tree.getChildWithName(ValTree_IDs::gui_params).
    getProperty("SaveDir").toString().toStdString();
  //Fetch last valid directory from valtree
  std::cout << "\tModeldir: " << guiconfig->modeldir << std::endl;
  loadModelList();

  // Fetch last loaded model from valtree and load it.
  // Then the combobox model bar should be able to get the 
  // last selected entry from the valtree at update().
  auto child_node = val_tree.getChildWithName(ValTree_IDs::gui_params);
  if (child_node.hasProperty ("SelectedID")){
    std::cout << "\tSelectedID: " << (int)child_node.getProperty("SelectedID") << std::endl;
    int last_model_to_load = ((int)child_node.getProperty("SelectedID")) - 1;
    reload_model(last_model_to_load);
    }
  }

}

void FMTTProcessor::showLoadDialog() {
  if (!builder_ptr) return;
  auto *guiconfig = magicState.getObjectWithType<PluginGUIConfig>("guiconfig");
  if(!guiconfig) return;
  auto dialog = std::make_unique<foleys::FileBrowserDialog>(
      NEEDS_TRANS("Cancel"), NEEDS_TRANS("Load"),
      juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectDirectories |
          juce::FileBrowserComponent::canSelectFiles,
      juce::File(guiconfig->modeldir),
      std::make_unique<juce::WildcardFileFilter>("*.ts", "*",
                                                 NEEDS_TRANS("Model Files")));
  dialog->setAcceptFunction([&, dlg = dialog.get(), guiconfig] {
    auto file = dlg->getFile();
    std::cout << "File: " << file.getFullPathName() << std::endl;
    if (file.getFileExtension().toStdString() == ".ts")
      guiconfig->modeldir =
          file.getParentDirectory().getFullPathName().toStdString();
    else
      guiconfig->modeldir = file.getFullPathName().toStdString();
    // Store dir at val tree
    storeToValTree(ValTree_IDs::gui_params,"SaveDir",juce::var(guiconfig->modeldir));
    loadModelList();
    builder_ptr->findGuiItemWithId(GUI_IDs::status)->update();
    builder_ptr->findGuiItemWithId(GUI_IDs::models)->update();
    builder_ptr->closeOverlayDialog();
  });
  dialog->setCancelFunction([&] {
    // Cancel Procedure
    builder_ptr->closeOverlayDialog();
  });

  builder_ptr->showOverlayDialog(std::move(dialog));
  return;
}
// Assumes a hidden GRU state of 128 for all models
void FMTTProcessor::loadModelList() {
  auto *guiconfig = magicState.getObjectWithType<PluginGUIConfig>("guiconfig");
  if(!guiconfig) return;
  guiconfig->modelfilenames.clear();
  guiconfig->modelnames.clear();
  guiconfig->nstates.clear();
  juce::File f(guiconfig->modeldir);
  auto modelList = f.findChildFiles(2, false, "*.ts");
  for (juce::File& file : modelList) {
    std::cout << " Listed " << file.getFileName() << std::endl;
    guiconfig->modelfilenames.push_back(file.getFileName().toStdString());
    guiconfig->modelnames.push_back(
        file.getFileNameWithoutExtension().toStdString());
    guiconfig->nstates.push_back(128);
  }
}

void FMTTProcessor::add_triggers() {
  magicState.addTrigger("load-model", [&] { showLoadDialog(); });
  magicState.addTrigger("print-valtree", [&] { printValueTree(); });
  return;
}

void FMTTProcessor::printValueTree() {

  auto &valueTree = magicState.getValueTree();
  std::cout << valueTree.toXmlString() << std::endl;
}


juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  auto algorithm = std::make_unique<juce::AudioProcessorParameterGroup>(
      "Algorithm", TRANS("Algorithm"), "|");
  algorithm->addChild(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID(IDs::algorithm, 1), "Algorithm", 1, 32, 1));

  // Frequency ratios in DX7 Format
  auto ratios_dx = std::make_unique<juce::AudioProcessorParameterGroup>(
      "FM Frequency Ratios", TRANS("Ratio between fundamental and oscillator frequencies"), "|");
  ratios_dx->addChild(
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmFine1, 1), "OSC1 FR Fine", 0, 99, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmCoarse1, 1), "OSC1 FR Coarse", 0, 31, 1),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmFine2, 1), "OSC2 FR Fine", 0, 99, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmCoarse2, 1), "OSC2 FR Coarse", 0, 31, 1),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmFine3, 1), "OSC3 FR Fine", 0, 99, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmCoarse3, 1), "OSC3 FR Coarse", 0, 31, 1),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmFine4, 1), "OSC4 FR Fine", 0, 99, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmCoarse4, 1), "OSC4 FR Coarse", 0, 31, 1),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmFine5, 1), "OSC5 FR Fine", 0, 99, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmCoarse5, 1), "OSC5 FR Coarse", 0, 31, 1),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmFine6, 1), "OSC6 FR Fine", 0, 99, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmCoarse6, 1), "OSC6 FR Coarse", 0, 31, 1));

  auto boost = std::make_unique<juce::AudioProcessorParameterGroup>(
      "FM Oscillator Boost", TRANS("Oscillator Level Boost"), "|");
  boost->addChild(
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmBoost1, 1), "OSC1 Boost", -12, 12, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmBoost2, 1), "OSC2 Boost", -12, 12, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmBoost3, 1), "OSC3 Boost", -12, 12, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmBoost4, 1), "OSC4 Boost", -12, 12, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmBoost5, 1), "OSC5 Boost", -12, 12, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::fmBoost6, 1), "OSC6 Boost", -12, 12, 0));

  auto gain = std::make_unique<juce::AudioProcessorParameterGroup>(
      "Gain", TRANS("Gain"), "|");
  gain->addChild(
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::inGain, 1), "Input Gain", -12, 12, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::outGain, 1), "Output Gain", -12, 12, 0));

  auto debug = std::make_unique<juce::AudioProcessorParameterGroup>(
      "Debug", TRANS("Debug"), "|");
  debug->addChild(
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug1, 1), "Console I/O", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug2, 1), "Bypass Inference", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug3, 1), "Audio Passthrough", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug5, 1), "Halve Pitch", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug6, 1), "Enable Reset", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug7, 1), "Enable OSC", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug8, 1), "Enable FeatReg", 0, 1, 0),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID(IDs::debug9, 1), "FeatReg Mode", 0, 1, 0));
  
  layout.add(std::move(algorithm), std::move(ratios_dx),
            std::move(boost), std::move(gain), std::move(debug));

  return layout;
}

void FMTTProcessor::configure_gui_listeners() {
  treeState.addParameterListener(IDs::algorithm, this);

  treeState.addParameterListener(IDs::fmBoost1, this);
  treeState.addParameterListener(IDs::fmBoost2, this);
  treeState.addParameterListener(IDs::fmBoost3, this);
  treeState.addParameterListener(IDs::fmBoost4, this);
  treeState.addParameterListener(IDs::fmBoost5, this);
  treeState.addParameterListener(IDs::fmBoost6, this);

  treeState.addParameterListener(IDs::fmCoarse1, this);
  treeState.addParameterListener(IDs::fmCoarse2, this);
  treeState.addParameterListener(IDs::fmCoarse3, this);
  treeState.addParameterListener(IDs::fmCoarse4, this);
  treeState.addParameterListener(IDs::fmCoarse5, this);
  treeState.addParameterListener(IDs::fmCoarse6, this);

  treeState.addParameterListener(IDs::fmFine1, this);
  treeState.addParameterListener(IDs::fmFine2, this);
  treeState.addParameterListener(IDs::fmFine3, this);
  treeState.addParameterListener(IDs::fmFine4, this);
  treeState.addParameterListener(IDs::fmFine5, this);
  treeState.addParameterListener(IDs::fmFine6, this);

  treeState.addParameterListener(IDs::inGain, this);
  treeState.addParameterListener(IDs::outGain, this);

  treeState.addParameterListener(IDs::debug1, this);
  treeState.addParameterListener(IDs::debug2, this);
  treeState.addParameterListener(IDs::debug3, this);
  treeState.addParameterListener(IDs::debug5, this);
  treeState.addParameterListener(IDs::debug6, this);
  treeState.addParameterListener(IDs::debug7, this);
  treeState.addParameterListener(IDs::debug8, this);
  treeState.addParameterListener(IDs::debug9, this);
}

void FMTTProcessor::parameterChanged(const juce::String& param, float value) {
  std::cout << param << " " << value << std::endl;
  // FM Configuration
  if (param == IDs::algorithm) {
    _config.fm_config = (int)value - 1;
  } 
  // FM Coarse
  else if (param == IDs::fmCoarse1) _config.fm_coarse[0] = value;
  else if (param == IDs::fmCoarse2) _config.fm_coarse[1] = value;
  else if (param == IDs::fmCoarse3) _config.fm_coarse[2] = value;
  else if (param == IDs::fmCoarse4) _config.fm_coarse[3] = value;
  else if (param == IDs::fmCoarse5) _config.fm_coarse[4] = value;
  else if (param == IDs::fmCoarse6) _config.fm_coarse[5] = value;

  // FM Fine
  else if (param == IDs::fmFine1) _config.fm_fine[0] = value;
  else if (param == IDs::fmFine2) _config.fm_fine[1] = value;
  else if (param == IDs::fmFine3) _config.fm_fine[2] = value;
  else if (param == IDs::fmFine4) _config.fm_fine[3] = value;
  else if (param == IDs::fmFine5) _config.fm_fine[4] = value;
  else if (param == IDs::fmFine6) _config.fm_fine[5] = value;

  // FM Oscillator Boost
  else if (param == IDs::fmBoost1) _config.fm_boost[0] = powf(10,value/20.0f);
  else if (param == IDs::fmBoost2) _config.fm_boost[1] = powf(10,value/20.0f);
  else if (param == IDs::fmBoost3) _config.fm_boost[2] = powf(10,value/20.0f);
  else if (param == IDs::fmBoost4) _config.fm_boost[3] = powf(10,value/20.0f);
  else if (param == IDs::fmBoost5) _config.fm_boost[4] = powf(10,value/20.0f);
  else if (param == IDs::fmBoost6) _config.fm_boost[5] = powf(10,value/20.0f);

  // Gain Values
  else if (param == IDs::inGain)  _config.in_gain = powf(10,value/20.0f);
  else if (param == IDs::outGain) _config.out_gain = powf(10,value/20.0f);
  // Debug Configuration
  else if (param == IDs::debug1)
    _config.enableConsoleOutput = (value != 0.0f);
  else if (param == IDs::debug2)
    _config.skipInference = (value != 0.0f);
  else if (param == IDs::debug3)
    _config.enableAudioPassthrough = (value != 0.0f);
  else if (param == IDs::debug5)
    _config.pitch_ratio = (value == 0.0f) ? 1.0f : 0.5f;
  else if (param == IDs::debug6)
    _config.allow_model_reset = (value != 0.0f);
  else if (param == IDs::debug7)
    _config.enableOSCOutput = (value != 0.0f);
  else if (param == IDs::debug8) {
    _feat_register.setState((value != 0.0f));
  } else if (param == IDs::debug9) {
    auto mode = (value == 0.0f) ? FeatureRegister::WorkingMode::SYNC
                                : FeatureRegister::WorkingMode::LATCH;
    _feat_register.setMode(mode);
  }
  apply_config();
  updateGuiConfig();
  return;
}
