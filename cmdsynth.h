#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "dr_wav.h"
#include "LowPassFilter.h"

class CmdSynth {
public:
    CmdSynth(float sampleRate, float cutoff, int waveType);
    CmdSynth(float sampleRate, float cutoff, int waveType, std::string filename);

    std::vector<float> generate_note(std::string frequencyStr, float duration);
    std::vector<drwav_int16> float_to_int16(std::vector<float>& buffer);
    std::vector<float> sequence_notes(std::string filename);
    std::vector<float> generate_single_note(std::string frequencyStr, float duration);

private:
    float GAIN;
    float sampleRate;
    float cutoff;
    int waveType;
    int sample;
    int prevSamples;
    float angle;
    bool prevSilence = false;
    LowPassFilter lpf;

    float decode_frequency(std::string frequencyStr);
    void apply_fade_in(std::vector<float>& buffer, int totalSamples, float fadeDuration);
    void apply_fade_out(std::vector<float>& buffer, int totalSamples, float fadeDuration);
};

