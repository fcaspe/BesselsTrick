#include "PluginProcessor.h"

#include <cmath>

/**
FMTTProcessor(): Constructor
    Here we create our attribute objects.
*/
FMTTProcessor::FMTTProcessor()
    : foleys::MagicProcessor(
          juce::AudioProcessor::BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      treeState(*this, nullptr, "PARAMETERS", createParameterLayout())

{
  // Main Plugin Setup Tasks come here.

  // 1. Create new class members.
  _pitch_tracker.reset(new Yin());
  _rms_processor.reset(new RMS_Processor());
  //_rms_processor_feedback.reset(new RMS_Processor());
  _fmsynth.reset(new FMSynth());

  // 2. Set GUI
  FOLEYS_SET_SOURCE_PATH(__FILE__);
  add_triggers();
  configure_gui_listeners();
  setupMeters();
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
FMTTProcessor(): Destructor
                             Here we delete our attribute objects.
*/
FMTTProcessor::~FMTTProcessor() {
  /*Kill Renderer*/
  _fmsynth.reset();
  _model.reset();
  _rms_processor.reset();
  _pitch_tracker.reset();
}

inline float normalize_pitch(float pitch) {
  if (pitch < 20.0f) return 0.0f;
  constexpr float midi_highest_note = 127.0f;
  constexpr float log_two = 0.69314718056f;
  const float midi_val = 12 * (logf(pitch / 220.0f) / log_two) + 57.01f;
  return midi_val / midi_highest_note;
}

/**
processBlock(): Rendering function

The same buffer is used for audio input and output.

*/
void FMTTProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                 juce::MidiBuffer& midiMessages) {
  /* Step1: Cleanup Tasks */
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  juce::AudioProcessLoadMeasurer::ScopedTimer s(_load_measurer);

  /* Step2: Gather control inputs */
  juce::ignoreUnused(midiMessages);

  constexpr int input_ch = 0;  // Use Channel 0 as input
  const float* raw_input = buffer.getReadPointer(input_ch);

  // Feedback
  for (auto sample = 0; sample < buffer.getNumSamples(); sample++) {
    _feedbackBuffer[sample] =
        raw_input[sample] + _feedbackBuffer[sample] * _config.feedback_level;
  }
  const float* audio_input = _feedbackBuffer.data();
  // RMS
  float rms_norm = _rms_processor->process(audio_input);
  if (_config.clamp_rms == true) {
    if (rms_norm > 0.0f) rms_norm = _config.rms_clamp_value;
  }
  float rms_in = rms_norm;
  // PITCH
  _pitch_tracker->updateBuffer(audio_input);
  float pitch = _pitch_tracker->getPitch();
  float prob = _pitch_tracker->getProbability();
  float pitch_norm = normalize_pitch(pitch);

  // Run Feature Register
  rms_in = _feat_register.run(pitch, rms_in);

  /* Step3: DNN inference */

  /* Reset logic */
  if (rms_in <= 0.0 && pitch_norm <= 0.0 && _config.allow_model_reset)
    if (_config.skipInference == false && _model) {
      _model->reset_state();
    }

  std::vector<float> fm_ol;
  std::vector<float> fm_ol_placeholder = {1, 0, 0, 0, 0, 0};
  if (_config.skipInference == false && _model) {
    fm_ol = _model->call(pitch_norm, rms_in);
  } else
    fm_ol = fm_ol_placeholder;

  // OP ENABLE
  for (int i = 0; i < _config.op_enable.size(); i++)
    if (_config.op_enable[i] == false) fm_ol[i] = 0.0f;

  /* DEBUG OVER CONSOLE */
  if (_config.enableConsoleOutput == true) {
    std::cout << std::endl << "[f0 " << pitch << "] [ld " << rms_norm << "]\n";
    for (int i = 0; i < 6; i++) std::cout << fm_ol[i] << " ";
    std::cout << std::endl;
  }

  if (_config.enableOSCOutput == true) {
    /* DEBUG OVER OSC */
    juce::OSCMessage msg1("/f0ld");
    msg1.addFloat32(pitch_norm);
    msg1.addFloat32(rms_in);
    msg1.addFloat32(prob);
    _osc_sender.send(msg1);

    // send envelopes over osc.
    juce::OSCMessage msg2("/fm_ol");
    for (float value : fm_ol) msg2.addFloat32(value);
    _osc_sender.send(msg2);

    if (_model) {
      if (_model->is_standalone() == false) {
        juce::OSCMessage msg3("/hidden");
        auto hidden_state = _model->get_state();
        for (float value : hidden_state) msg3.addFloat32(value);
        _osc_sender.send(msg3);
      }
    }

    juce::OSCMessage msg4("/fm_algo");
    // msg4.addInt32(*_guiconfig.fm_algorithm);
    _osc_sender.send(msg4);
  }

  /* Step4: Render audio */
  float* renderer_buffer = _fmsynth->render(pitch * _config.pitch_ratio, fm_ol);

  /* Step5: Update Meters*/
  if (outputLevel) outputLevel->pushSamples(buffer);
  // RMS FEEDBACK
  // rms_norm_feedback = _rms_processor_feedback->process(renderer_buffer);

  /* Step5: Store audio in JUCE's output buffers. */
  for (int i = 0; i < totalNumOutputChannels; i++) {
    auto* channelData = buffer.getWritePointer(i);
    if (_config.enableAudioPassthrough == false) {
      // Clear buffer with input audio.
      buffer.clear(i, 0, buffer.getNumSamples());
      for (auto sample = 0; sample < buffer.getNumSamples(); sample++) {
        channelData[sample] = renderer_buffer[sample];
        _feedbackBuffer[sample] = renderer_buffer[sample];
      }
    } else {
      for (auto sample = 0; sample < buffer.getNumSamples(); sample++) {
        channelData[sample] = audio_input[sample];  // Passthrough mode
      }
    }
  }
}

/**
prepareToPlay(): Reset function to configure plugin according to audio driver.
                Here we will reset and config all our objects before rendering.
*/
void FMTTProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  // Use this method as the place to do any pre-playback initialisation that you
  // need..

  std::cout << "[DEBUG] prepareToPlay() called!" << std::endl;
  _load_measurer.reset(sampleRate, samplesPerBlock);

  /* Init renderer */
  _feedbackBuffer.resize(samplesPerBlock);
  if (_fmsynth) _fmsynth->init(sampleRate, samplesPerBlock);

  /* Init RMS processor */
  const int RMS_WINDOW = 2048;
  if (_rms_processor)
    _rms_processor->init(RMS_WINDOW,        // Window size
                         samplesPerBlock);  // Block size

  /* Init Pitch Tracker */
  const int YIN_WINDOW = 1024;
  if (_pitch_tracker)
    _pitch_tracker->init(sampleRate, YIN_WINDOW, samplesPerBlock,
                         0.15f);  // Threshold
  if (outputLevel) outputLevel->setNumChannels(2);
}

//==============================================================================
const juce::String FMTTProcessor::getName() const { return JucePlugin_Name; }

bool FMTTProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool FMTTProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool FMTTProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double FMTTProcessor::getTailLengthSeconds() const { return 0.0; }

int FMTTProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int FMTTProcessor::getCurrentProgram() { return 0; }

void FMTTProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }

const juce::String FMTTProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void FMTTProcessor::changeProgramName(int index, const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void FMTTProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool FMTTProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
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
  return new FMTTProcessor();
}
