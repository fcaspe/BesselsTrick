#include "PluginProcessor.h"

void FMTTProcessor::apply_config() {
  _fmsynth->set_config(_config.fm_config);
  _fmsynth->set_ratios(_config.fm_ratios);
  _pitch_tracker->setThreshold(_config.yin_threshold);
}

void FMTTProcessor::reload_model(const unsigned int entry) {
  auto *guiconfig = magicState.getObjectWithType<PluginGUIConfig>("guiconfig");
  if(!guiconfig) return;

  // Ensure entry is within range
  if(entry > guiconfig->modelnames.size()) return;
  std::cout << "FMTTProcessor::reload_model() -  Loading "
            << (guiconfig->modelnames)[entry]
            << "  - state: " << guiconfig->nstates[entry] << std::endl;

  load_gru_model((guiconfig->modeldir) + "/" + guiconfig->modelfilenames[entry], guiconfig->nstates[entry]);

  std::cout << "\tContains patch:" << _model->contains_patch() << std::endl;
  if (_model->contains_patch()) {
    _fmsynth->load_dx7_config(_model->get_patch());
    _config.fm_config = _fmsynth->get_config();
    _config.fm_ratios = _fmsynth->get_ratios();
  }
}

// Load a standalone gru model
void FMTTProcessor::load_gru_model(const std::string& model_path) {
  load_gru_model(model_path,
                 0);  // Hidden size not specified for standalone model.
}

void FMTTProcessor::load_gru_model(const std::string& model_path,
                                   int n_state) {
  std::cout << "FMTTEditor - load_gru_model() " << model_path << " "
            << n_state << std::endl;
  bool old_skip_switch_val = _config.skipInference;
  _config.skipInference = true;  // Override config value
  _model.reset(new GRUModel());

  std::array<int, 3> model_input_sizes = {1, 1, 2};
  constexpr int n_outputs = 6;
  // Init for GRUModel
  _model->init(model_path, model_input_sizes,
               n_outputs, n_state);
  _config.skipInference = old_skip_switch_val;
}