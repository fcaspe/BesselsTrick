#include <memory>
#include <vector>
#include <array>
#include <string>
#include <juce_audio_processors/juce_audio_processors.h>

struct PluginGUIConfig
{
    std::vector<std::string> modelfilenames;    //Valid model filename list
    std::vector<std::string> modelnames;        //Valid modelname list
    std::vector<int> nstates;                   //Valid model statewidth list
    std::string modeldir = "~/";                //Directory listed for models
    std::string status = "";                    //Message from Status Label

    // FM Synth attributes for GUI display
    unsigned int fm_algo = 0;
    std::array<uint8_t, 6> fm_coarse = {1,1,1,1,1,1}; //FM Ratios Coarse
    std::array<uint8_t, 6> fm_fine = {0}; //FM Ratios Fine

};

struct PluginConfig
{
    int num_fmblocks;       // How many 64-sample FM blocks we fit in an audio block.
    float pitch_ratio;
    float yin_threshold;
    float rms_clamp_value;
    float feedback_level;
    float in_gain;
    float out_gain;
    bool portamento;
    bool infinite_sustain;
    bool allow_model_reset;
    bool enableOSCOutput;
    
    // FM Synth config
    unsigned int fm_config = 0;
    std::array<uint8_t, 6> fm_coarse = {1,1,1,1,1,1}; //FM Ratios Coarse
    std::array<uint8_t, 6> fm_fine = {0}; //FM Ratios Fine
    std::array<float, 6> fm_boost = 
        {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f}; //Osc Amplitude Boost


    //Function switches
    bool enableConsoleOutput;
    bool skipInference;
    bool enableAudioPassthrough;

    PluginConfig()
        {
        num_fmblocks = 0;
        pitch_ratio = 1.0f;
        yin_threshold = 0.15f;
        feedback_level = 0.0f;
        in_gain = 1.0f;
        out_gain = 1.0f;
        portamento = false;
        infinite_sustain = false;
        allow_model_reset = true;
        enableOSCOutput = true;
        enableConsoleOutput = false;
        skipInference = false;
        enableAudioPassthrough = false;
        }
};
