
#include "DynamicModels.hpp"

#include <cmath>

/*
    GRU based model
        Retains state internally - only requires the current conditioning value
*/

std::vector<float> GRUModel::call(float pitch, float loudness) {
  std::array<float, 2> inbuffer;
  inbuffer[0] = normalize_pitch(pitch);
  inbuffer[1] = normalize_loudness(loudness);

  if (_isStandalone)
    _torchmodel->call(inbuffer, _outbuffer);
  else
    _torchmodel->call(inbuffer, _outbuffer, _statebuffer);

  return _outbuffer;
}

std::vector<float> GRUModel::get_state() { return _statebuffer; }

void GRUModel::init(const std::string &filename,
                    std::array<int, 3> model_input_sizes, int n_outputs) {
  init(filename, model_input_sizes, n_outputs, 0);
}

bool GRUModel::reset_state() {
  if (_isStandalone == true)
    return false;
  else
    std::fill(_statebuffer.begin(), _statebuffer.end(), 0.0f);
  return true;
}

void GRUModel::init(const std::string &filename,
                    std::array<int, 3> model_input_sizes, int n_outputs,
                    int n_state) {
  InferenceConfig config;
  config.filename = filename;
  config.fixed_slices = {0, 0, 0};
  config.input_sizes = model_input_sizes;
  config.output_len = n_outputs;
  config.rolling_slice = 2;
  config.state_len = n_state;
  _isStandalone = (n_state > 0) ? false : true;
  _torchmodel->init(config);
  if (_torchmodel->contains_patch()) {
    _containsPatch = true;
    _torchmodel->get_patch(_initial_patch);
  }
  _outbuffer.resize(n_outputs);
  _statebuffer.resize(n_state);
}

DDSPModel::DDSPModel() { _torchmodel.reset(new TorchModel()); }

DDSPModel::~DDSPModel() {}

inline float DDSPModel::normalize_pitch(float pitch) {
  if (pitch < 20.0f) return 0.0f;
  constexpr float midi_highest_note = 127.0f;
  constexpr float log_two = 0.69314718056f;
  const float midi_val = 12 * (logf(pitch / 220.0f) / log_two) + 57.01f;
  return midi_val / midi_highest_note;
}

inline float DDSPModel::normalize_loudness(float loudness) {
  constexpr float _REF_DB = 20.7f;
  constexpr float _DB_RANGE = 80.0f;
  if (loudness < 1e-20f) loudness = 1e-20f;  // Avoid log instabilities
  float ld_squared_db = 20.0 * log10f(loudness);
  // printf("acc = %f rms_squared_db = %f \n",acc,rms_squared_db);
  /* Normalize */
  ld_squared_db -= _REF_DB;
  if (ld_squared_db < -_DB_RANGE) ld_squared_db = -_DB_RANGE;

  /* Scale [-DB_RANGE, 0] to [0, 1]. */
  return (ld_squared_db / _DB_RANGE) + 1.0f;
}
