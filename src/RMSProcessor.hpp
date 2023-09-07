class RMS_Processor {
 public:
  RMS_Processor() {
    _block_size = 256;
    _measure_count = 0;
    _n_entries = 0;
  }
  ~RMS_Processor();
  void init(int windowSize, int blockSize);
  float process(const float* frame);

 private:
  float* _rms_buffer =
      0;  // Buffer to store past measures size = (windowSize/blockSize)
  int _measure_count;  // Current Circular Buffer position.
  int _block_size;     // Block size (in samples).
  int _n_entries;      // Window size (in blocks).
  bool _use_linear_output = false;
};