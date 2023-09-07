/*
Wrapper for Torch inference
*/

#include <torch/script.h>  // One-stop header.

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct InferenceConfig {
  std::string filename;
  std::array<int, 3> input_sizes;  // Size of input tensor
  std::array<int, 3> fixed_slices;
  int rolling_slice =
      2;  // Slice ID to iterate through and extract output values
  int output_len = 128;  // Total length of output
  int state_len = 0;
};

class TorchModel {
 public:
  TorchModel();
  void init(InferenceConfig config);
  void get_patch(std::array<uint8_t, 156> &dest);
  bool contains_patch();
  void call(std::array<float, 2> input_array, std::vector<float> &output_array);
  void call(std::array<float, 2> input_array, std::vector<float> &output_array,
            std::vector<float> &state_array);

 private:
  torch::jit::script::Module _module;
  InferenceConfig _config = InferenceConfig();
  bool _isTypeES = false;
};