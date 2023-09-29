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
File: FMSynth.cpp

6-Operator FM synth implementation
*/
#include "FMSynth.hpp"

#include <cmath>
#include <iostream>
#define OP6 5
#define OP5 4
#define OP4 3
#define OP3 2
#define OP2 1
#define OP1 0

constexpr float TWO_PI = 2 * 3.14159265f;

FMSynth::FMSynth() {
  _config = 0;
  _t_step = 0;
  _block_size = 0;
  _previous_pitch_hz = 0;
  set_config(1);  //  Config for String Synth
}
FMSynth::~FMSynth() {
  //  free(_buffer); //No need. Vector deletes itself when out of scope.
}

void FMSynth::load_dx7_config(const std::array<uint8_t, 156> patch) {
  std::cout << "FMSynth::load_dx7_config()" << std::endl;
  uint8_t algorithm = patch[134];  // 0 - 31
  set_config(algorithm);
  for (int op = 0; op < 6; op++) {
    //  First in patch is OP6
    const int offset = op * 21;
    uint8_t is_fixed = patch[offset + 17];
    uint8_t f_coarse = patch[offset + 18];
    uint8_t f_fine = patch[offset + 19];
    uint8_t f_detune = patch[offset + 20];
    _fr_coarse[5-op] = f_coarse;
    _fr_fine[5-op] = f_fine;
    
    //  TODO: Add detune parameter +- 7 cents.
    float f = (f_coarse == 0) ? 0.5f : (float)f_coarse;
    _fr[5 - op] = f + (f / 100) * ((float)f_fine);
    // std::cout << "coarse " << (int)f_coarse  << " fine " << (int)f_fine << "
    // total: "<< _fr[5-op] << std::endl;
  }
  std::cout << "\t alg: " << (uint16_t)algorithm << " fr: " << _fr[0] << " "
            << _fr[1] << " " << _fr[2] << " " << _fr[3] << " " << _fr[4] << " "
            << _fr[5] << std::endl;
  _config = algorithm;
  //  uint8_t transpose = (patch[144]-24);
  //  double factor = 2^(((double)transpose)/12.0);
  //  fr = factor*fr
  return;
}

void FMSynth::set_ratios(std::array<uint8_t, 6> fr_coarse, 
    std::array<uint8_t, 6> fr_fine) 
{
  /* Load frequency rations */
  for (int op = 0; op < 6; op++) {
    _fr_coarse[op] = fr_coarse[op];
    _fr_fine[op] = fr_fine[op];
    
    //  TODO: Add detune parameter +- 7 cents.
    float f = (fr_coarse[op] == 0) ? 0.5f : (float)fr_coarse[op];
    _fr[op] = f + (f / 100) * ((float)fr_fine[op]);
  }
}

void FMSynth::set_config(unsigned int config) {
  if (config > 31) {
    throw("FMSynth::set_config - Invalid config value.");
  }
  memcpy(_outmatrix, _outm_table[config], sizeof(float) * 6);
  memcpy(_modmatrix, _mm_table[config], sizeof(int) * 36);

  // Now, normalize outmatrix values.
  float outval = 0.0f;
  for (int i = 0; i < 6; i++) outval += _outmatrix[i];

  outval = outval * 2;  // Max OL is 2 in dx7

  for (int i = 0; i < 6; i++) _outmatrix[i] /= outval;
  _config = config;  // Store value
}

unsigned int FMSynth::get_config() { return _config; }

std::array<uint8_t, 6> FMSynth::get_fr_coarse() { return _fr_coarse; }
std::array<uint8_t, 6> FMSynth::get_fr_fine() { return _fr_fine; }

void FMSynth::init(float sampleRate, int blockSize) {
  _buffer.resize(blockSize);
  for (int i = 0; i < 6; i++) _prev_ol[i] = 0.0f;
  _block_size = blockSize;
  _t_step = 1.0f / sampleRate;
  _previous_pitch_hz = 0.0f;
  reset_phase();
}

void FMSynth::reset_phase() {
  for (int i = 0; i < 6; i++) _phase[i] = 0.0f;
}

inline void FMSynth::fade_out() {
  float ramp = 1.0f;
  const float ramp_step = 1.0f / ((float)_block_size);
  for (int s = 0; s < _buffer.size(); s++) {
    _buffer[s] = _buffer[s] * ramp;  // Linear Fade
    //_buffer[s] = _buffer[s]* (ramp*ramp); //Concave quadratic fade.
    ramp -= ramp_step;
  }
}

inline void FMSynth::fade_in() {
  float ramp = 0.0f;
  const float ramp_step = 1.0f / ((float)_block_size);
  for (int s = 0; s < _buffer.size(); s++) {
    _buffer[s] = _buffer[s] * ramp;  // Linear Fade
    //_buffer[s] = _buffer[s]* (ramp * ramp); //Concave quadratic Fade
    ramp += ramp_step;
  }
}

inline void FMSynth::update_phase(float pitch_hz) {
  /* Update Phase accumulator */
  for (int i = 0; i < _phase.size(); i++) {
    _phase[i] += TWO_PI * pitch_hz * _fr[i] * _t_step;
    if (_phase[i] > TWO_PI) {
      _phase[i] -= TWO_PI;
    }
  }
}

float* FMSynth::render(float pitch_hz, std::vector<float> ol) {
  /* Compute ol increment for linear interpolation */
  std::array<float, 6> inc_ol;
  for (int i = 0; i < _prev_ol.size(); i++)
    inc_ol[i] = (ol[i] - _prev_ol[i]) / (float)(_block_size);

  /* Render silence if no freq info has been capture over two runs. */
  if (pitch_hz < 1.0f && _previous_pitch_hz < 1.0f) {
    std::fill(_buffer.begin(), _buffer.end(), 0.0f);
    reset_phase();
  }

  // if pitch_hz > 1 OR pitch_hz < 1 && _prev_pitch > 1
  else {
    // Select wether to render the last block's pitch (for fading out) of
    // current block.
    float used_pitch_hz = (pitch_hz < 1.0f) ? _previous_pitch_hz : pitch_hz;

    render_mm(used_pitch_hz, inc_ol);

    // If this is the first valid pitch value
    if (_previous_pitch_hz < 1.0f) {
      fade_in();
    }
  }

  /* If previous pitch was valid but now it's silence, fade out. */
  if (pitch_hz < 1.0f) {
    fade_out();
    reset_phase();
  }

  /* Store previous pitch and ol values. */
  _previous_pitch_hz = pitch_hz;
  std::copy_n(ol.begin(), _prev_ol.size(), _prev_ol.begin());  //_prev_ol = ol;
  return _buffer.data();
}

/* Render FM using modulation matrix */
void FMSynth::render_mm(float pitch_hz, std::array<float, 6> inc_ol) {
  std::array<float, 6> ol;
  ol = _prev_ol;
  const float scale = TWO_PI;  // standard scale for DX ( OLs go up to 2.0 )

  /*Plain auto is by value (you get a copy). Use auto&.*/
  for (auto& sample : _buffer) {
    // Create instant phase modulation registers
    std::array<float, 6> modphases;
    modphases = _phase;

    // Iterate through modulation matrix
    for (int mod_op = OP6; mod_op >= OP1; mod_op--) {
      for (int carr_op = OP1; carr_op <= OP6; carr_op++) {
        int mm_idx = carr_op * 6 + mod_op;  // modmatrix index
        // Select modulatior OPS
        if (_modmatrix[mm_idx]) {
          // std::cout << "Carrier OP" << carr_op+1 << " modulated by OP" <<
          // mod_op+1 << std::endl;
          const auto mod_ol = ol[mod_op];
          const float mod_output_to_carrier =
              sinf(modphases[mod_op]) * mod_ol * scale;
          modphases[carr_op] += mod_output_to_carrier;
        }
      }
    }

    /* Store sample - should use transform and a lambda expresion */
    sample = 0.0f;
    for (int i = OP1; i <= OP6; i++)
      sample += _outmatrix[i] * ol[i] * sinf(modphases[i]);

    /* Update phase and ol */
    update_phase(pitch_hz);

    // ol += inc_ol;
    std::transform(ol.begin(), ol.end(), inc_ol.begin(), ol.begin(),
                   std::plus<float>());
  }
}
