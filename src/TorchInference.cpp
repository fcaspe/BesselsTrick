
#include "TorchInference.hpp"

#include <cmath>

TorchModel::TorchModel() {}

void TorchModel::init(InferenceConfig config) {
  _config = config;
  _isTypeES = (_config.state_len == 0) ? false : true;
  {
  torch::NoGradGuard no_guard;  // Will only disable grads in current thread.
    try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    _module = torch::jit::load(_config.filename.c_str());
  } catch (const c10::Error &e) {
    std::cerr << "[LIBTORCH MODULE] error loading the model "
              << _config.filename << std::endl;
    return;
  }

  const std::string base_filename =
      _config.filename.substr(_config.filename.find_last_of("/\\") + 1);
  std::cout << "[LIBTORCH MODULE] " << base_filename << " loaded!\n";

  std::cout << "\tInput Size: [" << _config.input_sizes[0] << ", "
            << _config.input_sizes[1] << ", " << _config.input_sizes[2] << "]"
            << '\n';

  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(torch::ones({_config.input_sizes[0], _config.input_sizes[1],
                                _config.input_sizes[2]}));

  if (_isTypeES)
    // TODO: Not sure this works like so or we need an ivalue::Tuple
    inputs.push_back(torch::zeros({1, 1, config.state_len}));

  // Execute the model
  auto outputs = _module.forward(inputs);

  // Process its output.
  if (_isTypeES)  // Model is exposed state
  {
    // Output is an ivalue::tuple
    at::Tensor output = outputs.toTuple()->elements()[0].toTensor();
    std::cout << "\tOutput Size: [" << output.size(0) << ", " << output.size(1)
              << ", " << output.size(2) << "]" << '\n';

    at::Tensor state = outputs.toTuple()->elements()[1].toTensor();
    std::cout << "\tState Size: [" << state.size(0) << ", " << state.size(1)
              << ", " << state.size(2) << "]" << '\n';

    // std::cout << "[ES GRU] Model Output is tuple " << outputs.isTuple() <<
    // std::endl; std::cout << "[ES GRU] Print output[0]: " <<
    // outputs.toTuple()->elements()[0].toTensor() << std::endl; std::cout <<
    // "[ES GRU] Print output[1]: " <<
    // outputs.toTuple()->elements()[1].toTensor() << std::endl;
  } else  // model is standalone
  {
    at::Tensor output = outputs.toTensor();

    std::cout << "\tOutput Size: [" << output.size(0) << ", " << output.size(1)
              << ", " << output.size(2) << "]" << '\n';
  }
  }
}

bool TorchModel::contains_patch() {
  // Check if the module contains a training patch in buffer.
  const int n_buffers = _module.buffers().size();
  std::cerr << "[LIBTORCH MODULE] Module buffer count:" << n_buffers
            << std::endl;
  if (n_buffers > 0) return true;

  return false;
}

void TorchModel::get_patch(std::array<uint8_t, 156> &dest) {
  const int n_buffers = _module.buffers().size();
  if (n_buffers > 0) {
    //Print tensor
    //std::cout << "                  First buffer:\n"
    //          << *(_module.buffers().begin()) << std::endl;
    const int tensor_len = (*(_module.buffers().begin())).size(0);
    uint8_t *patch = (*_module.buffers().begin()).data_ptr<uint8_t>();
    std::copy(patch, patch + tensor_len, dest.begin());
    //std::cout << "\nCopied buffer: [";
    //for (uint8_t val : dest) std::cout << " " << static_cast<int16_t>(val);
    //std::cout << " ]\n";
  }
}
/*
Execute a forward pass for a standalone model
*/
void TorchModel::call(std::array<float, 2> input_array,
                      std::vector<float> &output_array) {
  {
  torch::NoGradGuard no_guard;  // Will only disable grads in current thread.
  /* Exposes the given data as a Tensor without taking ownership of the original
   * data. */
  at::Tensor input_tensor = torch::from_blob(
      input_array.data(),
      {_config.input_sizes[0], _config.input_sizes[1], _config.input_sizes[2]});

  // std::cout << "Input "<< input_tensor << '\n';
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(input_tensor);

  // Execute the model and turn its output (which is another jit::IValue) into a
  // tensor!
  at::Tensor output = _module.forward(inputs).toTensor();
  auto out_a = output.accessor<float, 3>();
  std::array<int, 3> access_indexes = _config.fixed_slices;

  for (int i = 0; i < output.size(_config.rolling_slice); i++) {
    /* Do not apply sigmoid activation and multiply by max_ol.
       This should be done within the torchscript now. */

    access_indexes[_config.rolling_slice] = i;
    output_array[i] =
        out_a[access_indexes[0]][access_indexes[1]][access_indexes[2]];
  }
  }
}

/*
Execute a forward pass for an exposed state model
*/
void TorchModel::call(std::array<float, 2> input_array,
                      std::vector<float> &output_array,
                      std::vector<float> &state_array) {
  // std::cout << "[DEBUG] Running call in ES model." << std::endl;
  
  {
  torch::NoGradGuard no_guard;  // Will only disable grads in current thread.
  /* Exposes the given data as a Tensor without taking ownership of the original
   * data. */
  at::Tensor input_tensor = torch::from_blob(
      input_array.data(),
      {_config.input_sizes[0], _config.input_sizes[1], _config.input_sizes[2]});

  at::Tensor state_tensor = torch::from_blob(
      state_array.data(),
      {_config.input_sizes[0], _config.input_sizes[1], _config.state_len});

  // std::cout << "Input "<< input_tensor << '\n';
  // std::cout << "State "<< state_tensor << '\n';
  //  TODO: Not sure this will work like so, or if we require to create an
  //  ivalue::Tuple.
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(input_tensor);
  inputs.push_back(state_tensor);

  torch::jit::IValue outputs;
  outputs = _module.forward(inputs);

  // TODO: Correct this. just placeholder.
  // Good info here:
  // https://github.com/pytorch/pytorch/blob/master/aten/src/ATen/core/ivalue.h
  at::Tensor model_out = outputs.toTuple()->elements()[0].toTensor();
  at::Tensor model_state = outputs.toTuple()->elements()[1].toTensor();

  // Copy output to vector
  auto out_a = model_out.accessor<float, 3>();
  std::array<int, 3> access_indexes = _config.fixed_slices;

  for (int i = 0; i < model_out.size(_config.rolling_slice); i++) {
    /* Do not apply sigmoid activation and multiply by max_ol.
       This should be done within the torchscript now. */

    access_indexes[_config.rolling_slice] = i;
    output_array[i] =
        out_a[access_indexes[0]][access_indexes[1]][access_indexes[2]];
  }

  // Copy state to vector
  out_a = model_state.accessor<float, 3>();
  for (int i = 0; i < model_state.size(_config.rolling_slice); i++) {
    access_indexes[_config.rolling_slice] = i;
    state_array[i] =
        out_a[access_indexes[0]][access_indexes[1]][access_indexes[2]];
  }
  }
}