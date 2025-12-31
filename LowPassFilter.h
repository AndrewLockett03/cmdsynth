//
// Created by Andrew Lockett on 12/30/25.
//

#ifndef CMDSYNTH_LOWPASSFILTER_H
#define CMDSYNTH_LOWPASSFILTER_H

#include <cmath>

class LowPassFilter {
public:
    LowPassFilter(float cutoff_freq = 20000, float sample_rate = 44100)
            : z1(0.0), z2(0.0) { // Initialize memory
        set_coefficients(cutoff_freq, sample_rate);
    }

    void set_coefficients(float cutoff_freq, float sample_rate);

    float update(float input);

private:
    float a0, a1, a2, b1, b2; // Coefficients
    float z1, z2;             // Filter memory (state variables)
};


#endif //CMDSYNTH_LOWPASSFILTER_H
