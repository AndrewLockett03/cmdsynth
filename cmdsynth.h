#include <cmath>
#include <iostream>
#include <vector>
#include "dr_wav.h"
#include "LowPassFilter.h"

class CmdSynth {
public:
    CmdSynth(float sampleRate, float cutoff, int waveType);

    drwav_int16* generate_note(char* frequencyStr, float duration);

private:
    float GAIN;
    float sampleRate;
    float cutoff;
    int waveType;
    LowPassFilter lpf;

    float decode_frequency(char* frequencyStr);
};

