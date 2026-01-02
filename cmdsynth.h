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
    float angle;
    LowPassFilter lpf;

    float decode_frequency(std::string frequencyStr);
};

