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
File: FeatureRegister.hpp: 

Feature register to sync RMS excitation with valid F0
It features two working modes:
    SYNC: Generates valid RMS only when f0 is present
    LATCH: GENERATES valid RMS when f0 is present,
        registering that value until the end of the f0.

If disabled, this module just returns the original rms.
*/
class FeatureRegister {
 public:
  enum State {
    DISABLED,
    WAITING,
    TRIGGERED,
  };
  enum WorkingMode { SYNC, LATCH };
  FeatureRegister() {
    _state = WAITING;
    _mode = SYNC;
    _registered_rms = 0.0f;
  }
  void setState(bool enable) {
    std::cout << "[FEATREG] State set: " << \
      ((enable)? "WAITING" : "DISABLED" )\
      << std::endl;
    _state = enable ? WAITING : DISABLED;
  }
  void setMode(WorkingMode mode) {
    std::cout << "[FEATREG] Mode set: " << \
      ((mode == SYNC)? "SYNC" : "LATCH")\
      << std::endl;
    _mode = mode;
  }
  float run(float f0, float rms) {
    float rms_return = 0.0f;
    switch (_state) {
      case DISABLED:
        rms_return = rms;
        break;
      case WAITING:
        if (f0 > 0.0f)  // Trigger condition
        {
          rms_return = rms;
          _registered_rms = rms;
          _state = TRIGGERED;
        } else  // No trigger
        {
          rms_return = 0.0f;
          _state = WAITING;
        }
        break;
      case TRIGGERED:
        if (f0 <= 0.0f) {
          rms_return = 0.0f;
          _state = WAITING;
        } else {
          if (_mode == SYNC)
            rms_return = rms;
          else if (_mode == LATCH)
            rms_return = _registered_rms;
        }
        break;
      default:
        break;
    }
    return rms_return;
  }

 private:
  State _state;
  WorkingMode _mode;
  float _registered_rms;
};
