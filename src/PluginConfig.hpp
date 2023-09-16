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
    std::string modeldir;                       //Directory listed for models
    int selectedid;                             //Selected id 
};

struct PluginConfig
{
    float pitch_ratio;
    float yin_threshold;
    float rms_clamp_value;
    float feedback_level;
    bool portamento;
    bool infinite_sustain;
    bool allow_model_reset;
    bool enableOSCOutput;
    //std::string model_path = "/Users/franco/aim/projs/vst/ddx7-vst/models/testmodels/testmodels";
    
    // FM Synth config
    std::array<float, 6> fm_ratios = {0};       //FM Ratios
    unsigned int fm_config = 0;
    //std::array<bool,6> op_enable = {true,true,true,true,true,true};

    //Function switches
    bool enableConsoleOutput;
    bool skipInference;
    bool enableAudioPassthrough;

    PluginConfig()
        {
        pitch_ratio = 1.0f;
        yin_threshold = 0.15f;
        feedback_level = 0.0f;
        portamento = false;
        infinite_sustain = false;
        allow_model_reset = true;
        enableOSCOutput = true;
        //fm_config = FMConfig::FM_ALG2_STRINGS;

        enableConsoleOutput = false;
        skipInference = false;
        enableAudioPassthrough = false;
        }
};
