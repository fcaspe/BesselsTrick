/*  Copyright 2023 <Franco Caspe> */

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
  void set_ratios(std::array<float, 6> fr);
  void set_config(unsigned int config);
  unsigned int get_config();
  std::array<float, 6> get_ratios();
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
