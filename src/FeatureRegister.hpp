/*
    Feature register to sync rms excitation with valid F0
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
    _state = DISABLED;
    _mode = SYNC;
    _registered_rms = 0.0f;
  }
  void setState(bool enable) {
    std::cout << "[FEATREG] State set: " << enable << std::endl;
    _state = enable ? WAITING : DISABLED;
  }
  void setMode(WorkingMode mode) {
    std::cout << "[FEATREG] Mode set: " << mode << std::endl;
    _mode = mode;
  }
  float run(float f0, float rms) {
    float rms_return = 0.0f;
    switch (_state) {
      case DISABLED:
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
