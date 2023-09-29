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
File: PitchTrackManager.hpp
Pitch tracking manager for multiscale pitch detection using multiple trackers.
*/

template <int num_units> class PitchTrackManager {
 public:
  PitchTrackManager() {}

  // Feed a bufferSizes array, from smaller to bigger
  void init(float sampleRate, std::array<int,num_units> bufferSizes, int frameSize,
            float yinThreshold, std::array<bool,num_units> downsample_x2)
  {
    for(int i = 0; i<num_units;i++)
    {
      _trackers[i]->init(sampleRate,bufferSizes[i],frameSize,yinThreshold,downsample_x2[i]);
      _limit_freqs[i] = 2*((10*sampleRate)/(bufferSizes[i]*9));
      std::cout << "[TRACK MGR] Buffer size is " << bufferSizes[i] \
        << " lim freq is " << _limit_freqs[i] << std::endl;
    }
  return;
  }

  void reset()
  {
    for(int i = 0; i<num_units;i++)
      _trackers[i].reset();
  }

  void create()
  {
    for(int i = 0; i<num_units;i++)
      _trackers[i].reset(new Yin());
  }
  void updateBuffer(const float* frame)
  {
    for(int i = 0; i<num_units;i++)
      _trackers[i]->updateBuffer(frame);
  }
  
  // Assumes classes are aranged from smaller to bigger
  // Computes the from the faster tracker, and if not available goes to the slower ones.
  float getPitch()
  {
    float pitch;
    for(int i = 0; i<num_units;i++)
    {
      pitch = _trackers[i]->getPitch();
      if(pitch != -1 && pitch !=0 & pitch > _limit_freqs[i])
        break;
    }
    return pitch;
  }
  
  void setThreshold(float threshold)
  {
    for(int i = 0; i<num_units;i++)
      _trackers[i]->setThreshold(threshold);
  }

 private:
    std::unique_ptr<Yin> _trackers[num_units];
    float _limit_freqs[num_units];
};
