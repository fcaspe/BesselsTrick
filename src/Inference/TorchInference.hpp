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
File: TorchInference.hpp
Wrapper for RNN inference using libtorch.
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