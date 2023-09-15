#include <cmath>

#include "DrawableLabel.hpp"
#include "PluginProcessor.h"

// ID's for objects in GUI
namespace GUI_IDs {
static juce::String models{"combobox_models"};
static juce::String algorithm{"knob_algo"};
static juce::String algoplot{"label_algo"};
static juce::String fmRatios1{"knob_fr1"};
static juce::String fmRatios2{"knob_fr2"};
static juce::String fmRatios3{"knob_fr3"};
static juce::String fmRatios4{"knob_fr4"};
static juce::String fmRatios5{"knob_fr5"};
static juce::String fmRatios6{"knob_fr6"};
}  // namespace GUI_IDs

// IDs for properties in ValueTree
namespace IDs {
static juce::String cboxselectedid{"cboxselectedid"};
static juce::String modeldir{"modeldir"};
static juce::String algorithm{"algorithm"};
static juce::String fmRatios1{"fmratios1"};
static juce::String fmRatios2{"fmratios2"};
static juce::String fmRatios3{"fmratios3"};
static juce::String fmRatios4{"fmratios4"};
static juce::String fmRatios5{"fmratios5"};
static juce::String fmRatios6{"fmratios6"};
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

/**
initialiseBuilder(): 
    Here we initialize the GUI Builder
*/
void FMTTProcessor::initialiseBuilder(foleys::MagicGUIBuilder& builder) {
  builder.registerJUCEFactories();
  builder.registerJUCELookAndFeels();
  
  builder.registerFactory("DrawableLabel", &DrawableLabelItem::factory);
  builder.registerFactory("ModelComboBox", &ModelComboBoxItem::factory);
  
  // Workaround to fetch builder from MagicPlugin without
  // overriding createEditor() (which should not be declared by plugin)
  builder_ptr = &builder;
}

void FMTTProcessor::setupMeters() {
  outputLevel =
      magicState.createAndAddObject<foleys::MagicLevelSource>("level");
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
  updateKnob(GUI_IDs::fmRatios1, _config.fm_ratios[0]);
  updateKnob(GUI_IDs::fmRatios2, _config.fm_ratios[1]);
  updateKnob(GUI_IDs::fmRatios3, _config.fm_ratios[2]);
  updateKnob(GUI_IDs::fmRatios4, _config.fm_ratios[3]);
  updateKnob(GUI_IDs::fmRatios5, _config.fm_ratios[4]);
  updateKnob(GUI_IDs::fmRatios6, _config.fm_ratios[5]);
}

/**
updateAlgoPlot(): 
    When algorithm changes, update graphic
*/
void FMTTProcessor::updateAlgoPlot(const int algo_num) {
  auto *alg = magicState.getObjectWithType<juce::var>("algorithm");
  if(alg != nullptr) *alg = algo_num;
  if (builder_ptr != nullptr)
    if (auto* item = builder_ptr->findGuiItemWithId(GUI_IDs::algoplot)) {
      if (auto* label =
              dynamic_cast<DrawableLabel*>(item->getWrappedComponent())) {
        label->setAlgorithm(algo_num);
      }
    }
}

/**
setupGUI(): 
    Setup GUI attributes (callbacks)
*/
void FMTTProcessor::setupGUI() {
  if (builder_ptr != nullptr)
    // Find ComboBox
    if (auto* item = builder_ptr->findGuiItemWithId(GUI_IDs::models)) {
      if (auto* combo_box =
              dynamic_cast<juce::ComboBox*>(item->getWrappedComponent())) {
        // Callback method
        combo_box->onChange = [&] {
          // Store state for gui recall
          auto* item = builder_ptr->findGuiItemWithId(GUI_IDs::models);
          auto* combo_box =
              dynamic_cast<juce::ComboBox*>(item->getWrappedComponent());

          const unsigned int selected_entry = combo_box->getSelectedId() - 1;
          std::cout << "Changed item:" << selected_entry << std::endl;
          // Stop Audio Processing Thread ( It can crash when re-loading model )
          suspendProcessing(true);
          reload_model(selected_entry);
          updateKnobs();
          suspendProcessing(false);
        };
      }
    }
}

/**
updateGUI(): 
    Refresh GUI
*/
void FMTTProcessor::updateGUI() {
  if (builder_ptr != nullptr) {
    // Find Combobox, clear it and refill with modelnames
    if (auto* item = builder_ptr->findGuiItemWithId(GUI_IDs::models)) 
      {
      std::cout << "Found ComboBox" << std::endl;
      if (auto* combo_box =
              dynamic_cast<juce::ComboBox*>(item->getWrappedComponent())) 
        {
        combo_box->clear(false);  // Remove all previous items
        int menu_idx = 1;
        for (std::string menuentry : _guiconfig.modelnames) 
          {
          combo_box->addItem(menuentry, menu_idx);
          menu_idx++;
          }
        combo_box->setSelectedId(1);  // Select first item
        }
      }
  }
}

void FMTTProcessor::setupValueTree()
{
  //std::cout << "[DEBUG] Setup Tree States" << std::endl;
  //treeState.state.setProperty(juce::Identifier(IDs::modeldir),"",nullptr);
  //treeState.state.setProperty(juce::Identifier(IDs::cboxselectedid),0,nullptr);
  magicState.createAndAddObject<juce::var>("algorithm");
  auto *alg = magicState.getObjectWithType<juce::var>("algorithm");
  if(alg == nullptr)
    std::cout << "[SETUP VAL TREE] Error creating Object";
  else
    *alg = 1;
}


void FMTTProcessor::postSetStateInformation()
{
  //std::cout << "[postSetStateInformation] Recalling from treeState" << std::endl;
  //int selected_model_id = treeState.state[juce::Identifier(IDs::cboxselectedid)];
  //juce::String model_dir = treeState.state[juce::Identifier(IDs::modeldir)];
  //_config.model_path = model_dir.toStdString();
  //loadModelList();
  //updateGUI();

}

void FMTTProcessor::showLoadDialog() {
  if (builder_ptr == 0) return;
  auto dialog = std::make_unique<foleys::FileBrowserDialog>(
      NEEDS_TRANS("Cancel"), NEEDS_TRANS("Load"),
      juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectDirectories |
          juce::FileBrowserComponent::canSelectFiles,
      juce::File(_config.model_path),
      std::make_unique<juce::WildcardFileFilter>("*.ts", "*",
                                                 NEEDS_TRANS("Model Files")));
  dialog->setAcceptFunction([&, dlg = dialog.get()] {
    auto file = dlg->getFile();
    if (file.getFileExtension().toStdString() == ".ts")
      _config.model_path =
          file.getParentDirectory().getFullPathName().toStdString();
    else
      _config.model_path = file.getFullPathName().toStdString();
    //treeState.state.setProperty(juce::Identifier(IDs::modeldir),
    //                juce::var(_config.model_path),
    //                nullptr);
    setupGUI();
    loadModelList();
    updateGUI();
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
  _guiconfig.modelfilenames.clear();
  _guiconfig.modelnames.clear();
  _guiconfig.nstates.clear();
  juce::File f(_config.model_path);
  auto modelList = f.findChildFiles(2, false, "*.ts");
  for (juce::File& file : modelList) {
    std::cout << " Listed " << file.getFileName() << std::endl;
    _guiconfig.modelfilenames.push_back(file.getFileName().toStdString());
    _guiconfig.modelnames.push_back(
        file.getFileNameWithoutExtension().toStdString());
    _guiconfig.nstates.push_back(128);
  }
}

void FMTTProcessor::add_triggers() {
  magicState.addTrigger("load-model", [&] { showLoadDialog(); });
  return;
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  auto algorithm = std::make_unique<juce::AudioProcessorParameterGroup>(
      "Algorithm", TRANS("Algorithm"), "|");
  algorithm->addChild(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID(IDs::algorithm, 1), "Algorithm", 1, 32, 1));

  auto ratios = std::make_unique<juce::AudioProcessorParameterGroup>(
      "Frequency Ratios", TRANS("Frequency Ratios"), "|");
  ratios->addChild(
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID(IDs::fmRatios1, 1), "OP1",
          juce::NormalisableRange<float>(0.5f, 20.0f, 0.01f), 1.0f),
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID(IDs::fmRatios2, 1), "OP2",
          juce::NormalisableRange<float>(0.5f, 20.0f, 0.01f), 1.0f),
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID(IDs::fmRatios3, 1), "OP3",
          juce::NormalisableRange<float>(0.5f, 20.0f, 0.01f), 1.0f),
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID(IDs::fmRatios4, 1), "OP4",
          juce::NormalisableRange<float>(0.5f, 20.0f, 0.01f), 1.0f),
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID(IDs::fmRatios5, 1), "OP5",
          juce::NormalisableRange<float>(0.5f, 20.0f, 0.01f), 1.0f),
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID(IDs::fmRatios6, 1), "OP6",
          juce::NormalisableRange<float>(0.5f, 20.0f, 0.01f), 1.0f));

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
  layout.add(std::move(algorithm), std::move(ratios), std::move(debug));

  return layout;
}

void FMTTProcessor::configure_gui_listeners() {
  treeState.addParameterListener(IDs::algorithm, this);

  treeState.addParameterListener(IDs::fmRatios1, this);
  treeState.addParameterListener(IDs::fmRatios2, this);
  treeState.addParameterListener(IDs::fmRatios3, this);
  treeState.addParameterListener(IDs::fmRatios4, this);
  treeState.addParameterListener(IDs::fmRatios5, this);
  treeState.addParameterListener(IDs::fmRatios6, this);

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
    updateAlgoPlot(int(value));
  } else if (param == IDs::fmRatios1)
    _config.fm_ratios[0] = value;
  else if (param == IDs::fmRatios2)
    _config.fm_ratios[1] = value;
  else if (param == IDs::fmRatios3)
    _config.fm_ratios[2] = value;
  else if (param == IDs::fmRatios4)
    _config.fm_ratios[3] = value;
  else if (param == IDs::fmRatios5)
    _config.fm_ratios[4] = value;
  else if (param == IDs::fmRatios6)
    _config.fm_ratios[5] = value;
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
  return;
}
