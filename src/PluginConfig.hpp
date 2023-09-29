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
File: PluginConfig.hpp
Declares config structures to store data related to plugin operation and GUI.
*/

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
