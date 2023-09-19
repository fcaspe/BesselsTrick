/*
    Pitch tracking manager for multiscale pitch detection
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
