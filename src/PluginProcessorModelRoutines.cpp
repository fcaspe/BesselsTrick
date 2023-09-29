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
File: PluginProcessorModelRoutines.cpp
Plugin routines for switching models and modifying FM patches
*/

#include "PluginProcessor.hpp"

void FMTTProcessor::apply_config() {
  _fmsynth->set_config(_config.fm_config);
  _fmsynth->set_ratios(_config.fm_coarse,_config.fm_fine);
  //_pitch_tracker->setThreshold(_config.yin_threshold);
  _tracker_manager.setThreshold(_config.yin_threshold);
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
    _config.fm_coarse = _fmsynth->get_fr_coarse();
    _config.fm_fine = _fmsynth->get_fr_fine();
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