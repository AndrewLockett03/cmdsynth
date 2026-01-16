//
// Created by Andrew Lockett on 12/29/25.
//

#include "dr_wav.h"
#include "cmdsynth.h"
#include <iostream>
#include <cmath>


CmdSynth::CmdSynth(float sr, float cf, int wt)
        : GAIN(0.316f),
          sampleRate(sr),
          cutoff(cf),
          waveType(wt),
          sample(0),
          angle(0.0f),
          lpf(cf, sr),
          prevSamples(0)
{}


float CmdSynth::decode_frequency(std::string frequencyStr) {
    int offset = 0;
    // Decode note name
    switch(frequencyStr[0]) {
        case 'A':
            offset = 12;
            break;
        case 'B':
            offset =14;
            break;
        case 'C':
            offset = 3;
            break;
        case 'D':
            offset = 5;
            break;
        case 'E':
            offset = 7;
            break;
        case 'F':
            offset = 8;
            break;
        case 'G':
            offset = 10;
            break;
        case 'S': // Silence
            return 0.0f;
        default:
            std::cerr << "Invalid note name. Use A-G.\n";
            return -1.0f;
    }

    // Check for accidental
    std::cout << frequencyStr[1] << "\n";
    int octave;
    if (frequencyStr[1] == '#' || frequencyStr[1] == 'b') {
        if (frequencyStr[1] == '#') {
            offset += 1;
        } else {
            offset -= 1;
        }
        frequencyStr += 1; // Move past accidental
        octave = frequencyStr[2] - '0'; // Get octave
    }
    else {
        octave = frequencyStr[1] - '0'; // Get octave
    }

    std::cout << "octave: " << octave << "\n";

    float frequency = 440 * pow(2, (offset + (octave - 5) * 12) / 12.0); // Calculate frequency
    std::cout << "Calculated frequency: " << frequency << " Hz\n";
    return frequency;
}


std::vector<float> CmdSynth::generate_note(std::string frequencyStr, float duration) {
    float frequency = decode_frequency(frequencyStr);
    int totalSamples = sampleRate * duration;
    std::vector<float> buffer(totalSamples);
    double theta;
    float temp = 0;

    if (frequency == 0.0f) {    // Generate Silence
        prevSilence = true;
        std::cout << "Previous samples: " << prevSamples << "\n";
        if (prevSamples > 0) {
            apply_fade_out(buffer, prevSamples, 1.0f); // Apply fade-out
        }
        std::cout << "Generating silence for " << duration << " seconds.\n";
        for (int i = 0; i < totalSamples; ++i) {
            buffer[i] = 0.0f;
        }
        std::cout << "Silence generated.\n";
    }
    else {
        std::cout << "Generating " << waveType << " wave at " << frequency << " Hz for " << duration << " seconds.\n";
        switch (waveType) {
            case 1:
                std::cout << "Waveform: Sine Wave\n";
                theta = 2 * M_PI * frequency / sampleRate;
                sample = static_cast<int>(angle / theta) - 1; // Continue from last angle
                for (int i = 0; i < totalSamples; ++i) {
                    angle = static_cast<float>(sample) * theta;
                    buffer[i] = sin(angle); // Default to sine wave
                    ++sample;
                }
                break;
            case 2:
                std::cout << "Waveform: Square Wave\n";
                theta = 2 * M_PI * frequency / sampleRate;
                sample = static_cast<int>(angle / theta) - 1; // Continue from last angle
                for (int i = 0; i < totalSamples; ++i) {
                    angle = static_cast<float>(sample) * theta;
                    buffer[i] = (sin(angle) > 0) ? 1.0f : -1.0f; // Square wave
                    ++sample;
                }
                break;
            case 3:
                std::cout << "Waveform: Sawtooth Wave\n";
                sample = static_cast<int>(angle / frequency * sampleRate) - 1; // Continue from last angle
                for (int i = 0; i < totalSamples; ++i) {
                    float spl_i = static_cast<float>(sample);
                    angle = spl_i * frequency / sampleRate;
                    buffer[i] = 2.0 * (angle - floor(angle)) - 1.0; // Sawtooth wave
                    ++sample;
                }
                break;
            case 4:
                std::cout << "Waveform: Triangle Wave\n";
                sample = static_cast<int>(angle / frequency * sampleRate) - 1; // Continue from last angle
                for (int i = 0; i < totalSamples; ++i) {
                    float d_i = static_cast<float>(sample);
                    angle = d_i * frequency / sampleRate;
                    temp = 2.0 * (angle - floor(angle)) - 1.0; // Sawtooth wave
                    buffer[i] = 2.0 * fabs(temp) - 1.0; // Convert to Triangle wave
                    ++sample;
                }
                break;
            default:
                std::cout << "Unknown waveform type. Defaulting to Sine Wave.\n";
                waveType = 1;
                break;
        }
        if (prevSilence) {
            apply_fade_in(buffer, totalSamples, 0.01f); // Apply fade-in
            prevSilence = false;
        }
    }
    prevSamples = totalSamples;
    return buffer;
}


void CmdSynth::apply_fade_in(std::vector<float>& buffer, int totalSamples, float fadeDuration) {
    std::cout << "Applying fade in...\n";
    if (fadeDuration <= 0.0f || buffer.empty()) return; // No fade to apply
    int fadeSamples = static_cast<int>(fadeDuration * sampleRate);  // Number of samples to fade
    if (fadeSamples > totalSamples) fadeSamples = totalSamples; // Cap fade samples to total samples
    for (int i = 0; i < fadeSamples && i < totalSamples; ++i) {
        buffer[i] *= static_cast<float>(i) / fadeSamples;
    }
}


void CmdSynth::apply_fade_out(std::vector<float>& buffer, int totalSamples, float fadeDuration) {
    std::cout << "Applying fade out...\n";
    if (fadeDuration <= 0.0f || buffer.empty()) return; // No fade to apply
    int fadeSamples = static_cast<int>(sampleRate * 0.01f);
    int start = std::max(0, (int)buffer.size() - fadeSamples);
    for (int i = start; i < buffer.size(); ++i) {
        buffer[i] *= float(buffer.size() - 1 - i) / fadeSamples;
    }
}


std::vector<drwav_int16> CmdSynth::float_to_int16(std::vector<float>& buffer) {
    int totalSamples = static_cast<int>(buffer.size());
    std::vector<drwav_int16> wav_buffer;
    for (int i = 0; i < totalSamples; ++i) {
        buffer[i] = lpf.update(buffer[i]);  // Apply low-pass filter to prevent aliasing
        drwav_int16 temp = static_cast<drwav_int16>(buffer[i] * 32767.0f * GAIN); // Scale to 16-bit PCM & apply gain
        wav_buffer.push_back(temp);
    }
    return wav_buffer;
}


std::vector<float> CmdSynth::sequence_notes(std::string filename) {
    std::ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return std::vector<float>();    // Return empty vector on error
    }
//    fin >> sampleRate >> cutoff >> waveType;
    std::string frequencyStr;
    float duration;
    std::vector<float> wav_vector;
    std::vector<float> temp_buffer;
    while (!fin.eof()) {
        fin >> std::ws >> frequencyStr >> duration;
        if (frequencyStr == "S" && !wav_vector.empty()) {
            apply_fade_out(wav_vector, prevSamples, 0.01f); // Apply fade-out before silence
        }
        temp_buffer = generate_note(frequencyStr, duration);
        wav_vector.insert(wav_vector.end(), temp_buffer.begin(), temp_buffer.end());
        temp_buffer.clear(); // Clear buffer after every note
    }
    // Close file
    fin.close();

    // Short silence at end
    temp_buffer = generate_note("S", 0.1f);
    wav_vector.insert(wav_vector.end(), temp_buffer.begin(), temp_buffer.end());
    temp_buffer.clear(); // Clear buffer

    return wav_vector;
}


std::vector<float> CmdSynth::generate_single_note(std::string frequencyStr, float duration) {
    std::vector<float> temp = generate_note(frequencyStr, duration);
    //Prevent popping with fade in and out
    apply_fade_in(temp, static_cast<int>(temp.size()), 0.01f); // Apply fade-in
    apply_fade_out(temp, static_cast<int>(temp.size()), 0.01f); // Apply fade-out
    return temp;
}

