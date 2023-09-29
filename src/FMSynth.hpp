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
File: FMSynth.hpp

6-Operator FM synth implementation
*/

#ifndef SRC_FMSYNTH_HPP_
#define SRC_FMSYNTH_HPP_
#endif  // SRC_FMSYNTH_HPP_

#include <array>
#include <vector>

#include "PluginConfig.hpp"
#include "algorithms.hpp"

class FMSynth {
 public:
  FMSynth();
  ~FMSynth();

  void init(float sampleRate, int blockSize);
  void set_ratios(std::array<uint8_t, 6> fr_coarse, std::array<uint8_t, 6> fr_fine);
  void set_config(unsigned int config);
  unsigned int get_config();
  std::array<uint8_t, 6> get_fr_fine();
  std::array<uint8_t, 6> get_fr_coarse();
  float* render(float pitch_hz, std::vector<float> ol);
  void load_dx7_config(const std::array<uint8_t, 156> patch);

 private:
  void update_phase(float pitch_hz);
  void reset_phase();
  void render_mm(float pitch_hz, std::array<float, 6> inc_ol);
  void fade_out();
  void fade_in();

  std::array<float, 6> _phase;
  std::array<float, 6> _prev_ol;
  std::array<float, 6> _fr;
  std::array<uint8_t, 6> _fr_fine;
  std::array<uint8_t, 6> _fr_coarse;
  float _previous_pitch_hz;
  unsigned int _config;
  int _block_size;
  float _t_step;
  std::vector<float> _buffer;

  int _modmatrix[36] = {0};
  float _outmatrix[6] = {0};

  std::array<const int*, 32> _mm_table = {
      ALG1_MM,  ALG2_MM,  ALG3_MM,  ALG4_MM,  ALG5_MM,  ALG6_MM,  ALG7_MM,
      ALG8_MM,  ALG9_MM,  ALG10_MM, ALG11_MM, ALG12_MM, ALG13_MM, ALG14_MM,
      ALG15_MM, ALG16_MM, ALG17_MM, ALG18_MM, ALG19_MM, ALG20_MM, ALG21_MM,
      ALG22_MM, ALG23_MM, ALG24_MM, ALG25_MM, ALG26_MM, ALG27_MM, ALG28_MM,
      ALG29_MM, ALG30_MM, ALG31_MM, ALG32_MM};
  std::array<const float*, 32> _outm_table = {
      ALG1_OUTM,  ALG2_OUTM,  ALG3_OUTM,  ALG4_OUTM,  ALG5_OUTM,  ALG6_OUTM,
      ALG7_OUTM,  ALG8_OUTM,  ALG9_OUTM,  ALG10_OUTM, ALG11_OUTM, ALG12_OUTM,
      ALG13_OUTM, ALG14_OUTM, ALG15_OUTM, ALG16_OUTM, ALG17_OUTM, ALG18_OUTM,
      ALG19_OUTM, ALG20_OUTM, ALG21_OUTM, ALG22_OUTM, ALG23_OUTM, ALG24_OUTM,
      ALG25_OUTM, ALG26_OUTM, ALG27_OUTM, ALG28_OUTM, ALG29_OUTM, ALG30_OUTM,
      ALG31_OUTM, ALG32_OUTM,
  };
};
