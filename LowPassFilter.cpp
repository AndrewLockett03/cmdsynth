//
// Created by Andrew Lockett on 12/30/25.
//

#include "LowPassFilter.h"
#include <cmath>

void LowPassFilter::set_coefficients(float cutoff_freq, float sample_rate) {
    // Use a standard Q value for Butterworth (approx 0.707)
    float Q = 0.707;
    float K = std::tan(M_PI * cutoff_freq / sample_rate);
    float norm = 1.0 / (1.0 + K / Q + K * K);

    a0 = K * K * norm;
    a1 = 2 * a0;
    a2 = a0;
    b1 = 2 * (K * K - 1) * norm;
    b2 = (1 - K / Q + K * K) * norm;
}

float LowPassFilter::update(float input) {
    // Biquad filter difference equation (Direct Form II transposed)
    float output = input * a0 + z1;
    z1 = input * a1 + z2 - b1 * output;
    z2 = input * a2 - b2 * output;
    return output;
}

