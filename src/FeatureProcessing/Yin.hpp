/*
File: YIN.hpp

YIN F0 Estimator, adapted by Franco Caspe for block-based operation.
Adapted from: https://github.com/JorenSix/Pidato

Original project license: GNU LESSER GENERAL PUBLIC LICENSE
                          Version 3, 29 June 2007

*/
#ifndef Yin_h
#define Yin_h

class Yin {
 public:
  Yin();
  ~Yin();
  void init(float sampleRate, int bufferSize, int frameSize,
            float yinThreshold, bool downsample_x2);
  void updateBuffer(const float* frame);
  float getPitch();
  float getProbability();
  void setThreshold(float threshold);

 private:
  float parabolicInterpolation(int tauEstimate);

 private:
  int absoluteThreshold();

 private:
  void cumulativeMeanNormalizedDifference();

 private:
  void difference();

 private:
  float _threshold;
  int _bufferSize;
  int _halfBufferSize;
  int _frameSize;
  int _writeIdx;
  float* _audioBuffer;
  float _sampleRate;
  float* _yinBuffer;
  float _probability;
  float _input_gain;
  int _fillCounter;
  bool _downsample_x2;
};

#endif
