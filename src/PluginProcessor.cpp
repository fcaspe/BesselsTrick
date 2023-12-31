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
File: PluginProcessor.cpp
Implements main audio processing and audio configuration functions for the plguin.
*/


#include "PluginProcessor.hpp"
#include <cmath>

/**
BesselsProcessor(): Constructor
    Here we create our attribute objects.
*/
BesselsProcessor::BesselsProcessor()
    : foleys::MagicProcessor(
          juce::AudioProcessor::BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      treeState(*this, nullptr, "PARAMETERS", createParameterLayout())

{
  // Main Plugin Setup Tasks come here.

  // 1. Create new class members.
  //_pitch_tracker.reset(new Yin());
  _tracker_manager.create();
  _rms_processor.reset(new RMS_Processor());
  //_rms_processor_feedback.reset(new RMS_Processor());
  _fmsynth.reset(new FMSynth());

  // 2. Set GUI
  FOLEYS_SET_SOURCE_PATH(__FILE__);
  add_triggers();
  configure_gui_listeners();
  setupMeters();
  setupValueTree();
  // juce::File gui_tree(
  //  "/Users/franco/aim/projs/vst/ddx7-vst/resources/magic.xml");
  // magicState.setGuiValueTree(gui_tree);
  magicState.setGuiValueTree (BinaryData::magic_bake_xml,
    BinaryData::magic_bake_xmlSize);

  // OSC Live Debug Module
  if (_osc_sender.connect("127.0.0.1", 12000))  // [4]
    std::cout << "[OSC] Error: could not connect to UDP port 12000."
              << std::endl;
  else
    std::cout << "[OSC] Connected: UDP port 12000." << std::endl;
}

/**
BesselsProcessor(): Destructor
                             Here we delete our attribute objects.
*/
BesselsProcessor::~BesselsProcessor() {
  /*Kill Renderer*/
  _fmsynth.reset();
  _model.reset();
  _rms_processor.reset();
  //_pitch_tracker.reset();
  _tracker_manager.reset();
}

inline float normalize_pitch(float pitch) {
  if (pitch < 20.0f) return 0.0f;
  constexpr float midi_highest_note = 127.0f;
  constexpr float log_two = 0.69314718056f;
  const float midi_val = 12 * (logf(pitch / 220.0f) / log_two) + 57.01f;
  return midi_val / midi_highest_note;
}

inline void BesselsProcessor::sendDebugMessages(int fmblock,float pitch,
  float pitch_norm,float rms_in, std::vector<float> fm_ol)
{
  /* DEBUG OVER CONSOLE */
  if (_config.enableConsoleOutput == true) {
    std::cout << "[fmblock " << fmblock << "] [f0 " << pitch << "] [ld " << rms_in << "]\n";
    for (int i = 0; i < 6; i++) std::cout << fm_ol[i] << " ";
    std::cout << std::endl;
  }

  /* Step7: Debug*/
  if (_config.enableOSCOutput == true) {
    /* DEBUG OVER OSC */
    juce::OSCMessage msg1("/f0ld");
    msg1.addFloat32(pitch_norm);
    msg1.addFloat32(rms_in);
    msg1.addFloat32(0.0f); //Probability placeholder. Don't send anymore
    _osc_sender.send(msg1);

    // send envelopes over osc.
    juce::OSCMessage msg2("/fm_ol");
    for (float value : fm_ol) msg2.addFloat32(value);
    _osc_sender.send(msg2);

    //if (_model) {
    //  if (_model->is_standalone() == false) {
    //    juce::OSCMessage msg3("/hidden");
    //    auto hidden_state = _model->get_state();
    //    for (float value : hidden_state) msg3.addFloat32(value);
    //    _osc_sender.send(msg3);
    //  }

    //juce::OSCMessage msg4("/fm_algo");
    // msg4.addInt32(*_guiconfig.fm_algorithm);
    //_osc_sender.send(msg4);
    }
}

/**
processBlock(): Rendering function

The same buffer is used for audio input and output.

*/
void BesselsProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                 juce::MidiBuffer& midiMessages) {
  /* Step1: Cleanup Tasks */
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  juce::AudioProcessLoadMeasurer::ScopedTimer s(_load_measurer);
  juce::ignoreUnused(midiMessages);
  
  const int input_ch = 0;  // Use Channel 0 as input
  
  // Compute global f0 for all fmblocks to be synthesized.
  _tracker_manager.updateBuffer(buffer.getReadPointer(input_ch));
  float pitch = _tracker_manager.getPitch();
  float pitch_norm = normalize_pitch(pitch);

  // Each fm block renders 64 samples, adapt to the selected plugin block size.
  for(int fmblock = 0; fmblock < _config.num_fmblocks ; fmblock++)
  {
    const int start_sample = fm_block_size*fmblock;

    auto* input_read_ptr = buffer.getWritePointer(input_ch,start_sample);
    auto *audio_input = buffer.getReadPointer(input_ch,start_sample);
    // Input Gain
    for (auto sample = 0; sample < fm_block_size; sample++) {
      input_read_ptr[sample] = input_read_ptr[sample] * _config.in_gain;
    }

    /* Step2: Gather control inputs */
    // RMS
    float rms_in = _rms_processor->process(audio_input);

    // Run Feature Register
    rms_in = _feat_register.run(pitch, rms_in);

    /* Step3: DNN inference */

    /* DNN Reset logic */
    if(_config.allow_model_reset)
      if (rms_in <= 0.0 && pitch_norm <= 0.0)
        if (_config.skipInference == false && _model) {
          _model->reset_state();
        }

    std::vector<float> fm_ol;
    std::vector<float> fm_ol_placeholder = {1, 0, 0, 0, 0, 0};
    if (_model && _config.skipInference == false) {
      fm_ol = _model->call(pitch_norm, rms_in);
    } else
      fm_ol = fm_ol_placeholder;
    // FM Boost
    for (int i = 0; i < 6; i++)
      fm_ol[i] = fm_ol[i] * _config.fm_boost[i];

    /* Step4: Render audio */
    float* renderer_buffer = _fmsynth->render(pitch * _config.pitch_ratio, fm_ol);

    sendDebugMessages(fmblock,pitch,pitch_norm,rms_in,fm_ol);
    _fm_render_buffer.copyFrom(0,               //Dest Channel
                               start_sample,    //Dest Start Sample
                               renderer_buffer, // Source
                               fm_block_size);  //N Samples
    
    // If this is the last FM block, update GUI meters
    if(fmblock == _config.num_fmblocks - 1)
      updateMeters(rms_in,pitch,_fm_render_buffer);
  } // end for loop fmblock

  /* Step5: Store audio in JUCE's output buffers. */
  for (int i = 0; i < totalNumOutputChannels; i++) {
    auto* channelData = buffer.getWritePointer(i);
    // Our fm render buffer contains just one channel
    auto *renderData = _fm_render_buffer.getReadPointer(0,0);
    if (_config.enableAudioPassthrough == false) {
      // Clear buffer with input audio.
      buffer.clear(i, 0, buffer.getNumSamples());
      for (auto sample = 0; sample < buffer.getNumSamples(); sample++) {
        channelData[sample] = renderData[sample] * _config.out_gain;
      }
    }
  }

}

/**
prepareToPlay(): Reset function to configure plugin according to audio driver.
                Here we will reset and config all our objects before rendering.
*/
void BesselsProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  // Use this method as the place to do any pre-playback initialisation that you
  // need..

  std::cout << "[DEBUG] prepareToPlay() called!" << std::endl;
  _load_measurer.reset(sampleRate, samplesPerBlock);

  _config.num_fmblocks = samplesPerBlock / fm_block_size;
  _fm_render_buffer.setSize(1,samplesPerBlock);
  /* Init renderer */
  //_feedbackBuffer.resize(samplesPerBlock);
  if (_fmsynth) _fmsynth->init(sampleRate, fm_block_size);

  /* Init RMS processor */
  const int RMS_WINDOW = 2048;
  if (_rms_processor)
    _rms_processor->init(RMS_WINDOW,    // Window size
                         fm_block_size, // Block size
                         true);         // Linear output

  /* Init Pitch Trackers */
  // Minimum f0 detectable: 2*(sr/yinwindow)
  const std::array<int,4> yin_windows = {256,512,1024,1280};
  const std::array<bool,4> yin_downsample = {false,false,false,false};
  _tracker_manager.init(sampleRate, yin_windows, samplesPerBlock,
                        0.15f, // Threshold
                        yin_downsample); // Downsample x2
}

//==============================================================================
const juce::String BesselsProcessor::getName() const { return JucePlugin_Name; }

bool BesselsProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool BesselsProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool BesselsProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double BesselsProcessor::getTailLengthSeconds() const { return 0.0; }

int BesselsProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int BesselsProcessor::getCurrentProgram() { return 0; }

void BesselsProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }

const juce::String BesselsProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void BesselsProcessor::changeProgramName(int index, const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void BesselsProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool BesselsProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new BesselsProcessor();
}
