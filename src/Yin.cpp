/*
File: YIN.cpp

adapted by Franco Caspe for block-based operation.
Adapted from: https://github.com/JorenSix/Pidato

Original project license: GNU LESSER GENERAL PUBLIC LICENSE
                          Version 3, 29 June 2007

*/
#include "Yin.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

void Yin::init(float yinSampleRate, int yinBufferSize, int frameSize,
               float yinThreshold, bool downsample_x2) {
  _downsample_x2 = downsample_x2;
  const int downsampleFactor = _downsample_x2 ? 2 : 1;
  _sampleRate = yinSampleRate / downsampleFactor;
  _bufferSize = yinBufferSize;
  _frameSize = frameSize;
  _halfBufferSize = _bufferSize / 2;
  _probability = 0.0;
  _writeIdx = 0;
  _input_gain = 1.0f;

  // Counter that specifies how many frames to take before
  // starting to compute yin
  if ((_frameSize / downsampleFactor) < yinBufferSize)
    _fillCounter = (yinBufferSize / (_frameSize / downsampleFactor)) - 1;
  else
    _fillCounter = 0;  // A single frame will fill yin buffer.

  // std::cout << "[DEBUG] YIN frame size " << _frameSize << " buffer size " <<
  // _bufferSize << std::endl; initialize array and set it to zero
  free(_audioBuffer);
  _audioBuffer = (float *)malloc(sizeof(float) * _bufferSize);
  for (int i = 0; i < _bufferSize; i++) {
    _audioBuffer[i] = 0;
  }
  free(_yinBuffer);
  _yinBuffer = (float *)malloc(sizeof(float) * _halfBufferSize);
  for (int i = 0; i < _halfBufferSize; i++) {
    _yinBuffer[i] = 0;
  }

  setThreshold(yinThreshold);  // Default is 0.15
}

Yin::Yin() {
  _audioBuffer = (float *)NULL;
  _yinBuffer = (float *)NULL;
}

Yin::~Yin() {
  free(_audioBuffer);
  free(_yinBuffer);
}

float Yin::getProbability() { return _probability; }

float Yin::getPitch() {
  // Make sure we have some data in the audio buffer
  float acc = 0;
  for (int i = 0; i < _bufferSize; i++) {
    acc += fabs(_audioBuffer[i]);
  }
  if (acc < 0.01f) return -1;

  if (_fillCounter != 0) {
    _fillCounter--;
    return -1;
  }

  int tauEstimate = -1;
  float pitchInHertz = 0;

  // step 2
  difference();

  // step 3
  cumulativeMeanNormalizedDifference();

  // step 4
  tauEstimate = absoluteThreshold();

  // step 5
  if (tauEstimate != -1) {
    pitchInHertz = _sampleRate / parabolicInterpolation(tauEstimate);
  }

  return pitchInHertz;
}

void Yin::setThreshold(float threshold) { _threshold = threshold; }

float Yin::parabolicInterpolation(int tauEstimate) {
  float betterTau;
  int x0;
  int x2;

  if (tauEstimate < 1) {
    x0 = tauEstimate;
  } else {
    x0 = tauEstimate - 1;
  }
  if (tauEstimate + 1 < _halfBufferSize) {
    x2 = tauEstimate + 1;
  } else {
    x2 = tauEstimate;
  }
  if (x0 == tauEstimate) {
    if (_yinBuffer[tauEstimate] <= _yinBuffer[x2]) {
      betterTau = tauEstimate;
    } else {
      betterTau = x2;
    }
  } else if (x2 == tauEstimate) {
    if (_yinBuffer[tauEstimate] <= _yinBuffer[x0]) {
      betterTau = tauEstimate;
    } else {
      betterTau = x0;
    }
  } else {
    float s0, s1, s2;
    s0 = _yinBuffer[x0];
    s1 = _yinBuffer[tauEstimate];
    s2 = _yinBuffer[x2];
    // fixed AUBIO implementation, thanks to Karl Helgason:
    // (2.0f * s1 - s2 - s0) was incorrectly multiplied with -1
    betterTau = tauEstimate + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
  }
  return betterTau;
}

void Yin::cumulativeMeanNormalizedDifference() {
  int tau;
  _yinBuffer[0] = 1;
  float runningSum = 0;
  for (tau = 1; tau < _halfBufferSize; tau++) {
    runningSum += _yinBuffer[tau];
    _yinBuffer[tau] *= tau / runningSum;
  }
}

void Yin::updateBuffer(const float *frame) {
  // std::cout << "\nupdate buffer - framesize " << _frameSize << std::endl;
  // std::cout << "\n writeidx start: " << _writeIdx << std::endl;
  const int downsampleFactor = _downsample_x2 ? 2 : 1;
  for (int i = 0; i < _frameSize/downsampleFactor; i++) {
    const int idx = (i + _writeIdx) % _bufferSize;
    
    if(_downsample_x2)
      _audioBuffer[idx] = _input_gain * (frame[2*i] + frame[2*i+1]) / 2.0f;
    else
      _audioBuffer[idx] = _input_gain * frame[i];
    // std::cout << idx << " ";
    // std::cout << _audioBuffer[idx] << " ";
  }
  _writeIdx = (_writeIdx + (_frameSize/downsampleFactor)) % _bufferSize;
  // std::cout << "\n writeidx end: " << _writeIdx << std::endl;
}

void Yin::difference() {
  int index;
  int tau;
  float delta;
  // std::cout << "\n\ndifference\n";
  for (tau = 0; tau < _halfBufferSize; tau++) {
    // std::cout << "\ndly " << tau << std::endl;
    for (index = 0; index < _halfBufferSize; index++) {
      /*Compute indexes for circular buffer.*/
      const int idx = (index + _writeIdx) % _bufferSize;
      const int idx_plus_tau = (idx + tau) % _bufferSize;
      // std::cout << " " << idx << " - " << idx_plus_tau << " ";
      // std::cout << "(" << _audioBuffer[idx] << ")" << "[" <<
      // _audioBuffer[idx_plus_tau] << "] ";
      delta = _audioBuffer[idx] - _audioBuffer[idx_plus_tau];
      _yinBuffer[tau] += delta * delta;
    }
    // std::cout << _yinBuffer[tau] << "-- ";
  }
}

int Yin::absoluteThreshold() {
  int tau;
  // first two positions in yinBuffer are always 1
  // So start at the third (index 2)
  // std::cout << "\nabs threshold\n";
  for (tau = 2; tau < _halfBufferSize; tau++) {
    // std::cout << _yinBuffer[tau] << " ";
    if (_yinBuffer[tau] < _threshold) {
      while (tau + 1 < _halfBufferSize &&
             _yinBuffer[tau + 1] < _yinBuffer[tau]) {
        tau++;
      }
      // found tau, exit loop and return
      // store the probability
      // From the YIN paper: The threshold determines the list of
      // candidates admitted to the set, and can be interpreted as the
      // proportion of aperiodic power tolerated
      // within a ëëperiodicíí signal.
      //
      // Since we want the periodicity and and not aperiodicity:
      // periodicity = 1 - aperiodicity
      _probability = 1 - _yinBuffer[tau];
      break;
    }
  }
  // if no pitch found, tau => -1
  if (tau == _halfBufferSize || _yinBuffer[tau] >= _threshold) {
    tau = -1;
    _probability = 0;
  }
  return tau;
}
