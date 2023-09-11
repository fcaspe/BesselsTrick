#ifndef Yin_h
#define Yin_h

/*
YIN F0 Estimator, frame by frame operation.

Adapted from: https://github.com/JorenSix/Pidato
*/

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
