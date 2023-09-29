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
File: EnvModels.hpp: 
Introuduce wrapper classes to hide Libtorch implementation of
the Envelope RNNs.
*/

#include "TorchInference.hpp" 

class EnvModel {
 public:
  virtual void init(const std::string &filename,
                    std::array<int, 3> model_input_sizes, int n_outputs) = 0;
  virtual void init(const std::string &filename,
                    std::array<int, 3> model_input_sizes, int n_outputs,
                    int n_state) = 0;
  virtual std::vector<float> get_state() = 0;
  virtual std::vector<float> call(float pitch, float loudness) = 0;
  virtual bool reset_state() = 0;
  virtual ~EnvModel();
  bool is_standalone() { return _isStandalone; }
  bool contains_patch() { return _containsPatch; }
  std::array<uint8_t, 156> get_patch() { return _initial_patch; }

 protected:
  EnvModel();
  std::unique_ptr<TorchModel> _torchmodel;
  // We have to use vector cause we dont know the size yet until init is called
  std::vector<float> _outbuffer;
  std::vector<float> _statebuffer;
  bool _isStandalone = false;
  bool _containsPatch = false;
  float normalize_pitch(float pitch);
  float normalize_loudness(float loudness);
  std::array<uint8_t, 156> _initial_patch = {0};
};

class GRUModel : public EnvModel {
 public:
  std::vector<float> call(float pitch, float loudness) override;
  std::vector<float> get_state() override;
  bool reset_state() override;
  void init(const std::string &filename, std::array<int, 3> model_input_sizes,
            int n_outputs) override;
  void init(const std::string &filename, std::array<int, 3> model_input_sizes,
            int n_outputs, int n_state) override;
};
