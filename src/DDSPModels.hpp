#include "TorchInference.hpp"

/*
DDSPModels.hpp: Introuduce wrapper classes to hide Libtorch implementation of
the DDSP/DDX7 model.

    Different DDSP-like models (either GRU or TCN ) require different I/O
handling over libtorch tensors. We'll hide the implementation in this class.

*/

class DDSPModel {
 public:
  virtual void init(const std::string &filename,
                    std::array<int, 3> model_input_sizes, int n_outputs) = 0;
  virtual void init(const std::string &filename,
                    std::array<int, 3> model_input_sizes, int n_outputs,
                    int n_state) = 0;
  virtual std::vector<float> get_state() = 0;
  virtual std::vector<float> call(float pitch, float loudness) = 0;
  virtual bool reset_state() = 0;
  virtual ~DDSPModel();
  bool is_standalone() { return _isStandalone; }
  bool contains_patch() { return _containsPatch; }
  std::array<uint8_t, 156> get_patch() { return _initial_patch; }

 protected:
  DDSPModel();
  std::unique_ptr<TorchModel> _torchmodel;
  // We have to use vector cause we dont know the size yet until init is called
  std::vector<float> _outbuffer;
  std::vector<float> _statebuffer;
  bool _isStandalone = false;
  bool _containsPatch = false;
  float normalize_pitch(float pitch);
  float normalize_loudness(float loudness);
  std::array<uint8_t, 156> _initial_patch = {0};
};

class GRUModel : public DDSPModel {
 public:
  std::vector<float> call(float pitch, float loudness) override;
  std::vector<float> get_state() override;
  bool reset_state() override;
  void init(const std::string &filename, std::array<int, 3> model_input_sizes,
            int n_outputs) override;
  void init(const std::string &filename, std::array<int, 3> model_input_sizes,
            int n_outputs, int n_state) override;
};
