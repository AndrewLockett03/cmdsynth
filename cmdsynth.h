#include <cmath>
#include <iostream>
#include <vector>
#include "dr_wav.h"
#include "LowPassFilter.h"

class CmdSynth {
public:
    CmdSynth(float sampleRate, float cutoff, int waveType);

    drwav_int16* generate_note(std::string frequencyStr, float duration);

private:
    float GAIN;
    float sampleRate;
    float cutoff;
    int waveType;
    int sample;
    int prevSamples;
    float angle;
    bool prevSilence = true;
    LowPassFilter lpf;

    float decode_frequency(std::string frequencyStr);
    void apply_fade_in_out(float* buffer, int totalSamples, float fadeDuration, int inOutSwitch);
};

