//
// Created by Andrew Lockett on 12/29/25.
//

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include "LowPassFilter.h"
#include <iostream>
#include <cmath>


int main(int argc, char** argv) {

    const float GAIN = 0.316f; // Overall gain factor

    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <sample_rate> <frequency> <duration> <waveform_type> <cutoff>\n";
        return 1;
    }

    float sampleRate = atof(argv[1]); // Sample rate from command line
    char* frequencyStr = argv[2];   // Note name from command line
    float duration = atof(argv[3]);   // Duration from command line
    int waveType = atoi(argv[4]);       // Waveform type 1-4 from command line
    float cutoff = atof(argv[5]);   // Cutoff frequency for low-pass filter

    int offset = 0;
    // Decode note name
    switch(frequencyStr[0]) {
        case 'A':
            offset = 0;
            break;
        case 'B':
            offset = 2;
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
        default:
            std::cerr << "Invalid note name. Use A-G.\n";
            return 1;
    }

    // Check for accidental
    if (frequencyStr[1] == '#' || frequencyStr[1] == 'b') {
        if (frequencyStr[1] == '#') {
            offset += 1;
        } else {
            offset -= 1;
        }
        frequencyStr += 1; // Move past accidental
    }

    int octave = atoi(&frequencyStr[1]);
    float frequency = 440 * pow(2, (offset + (octave - 5) * 12) / 12.0); // Calculate frequency
    std::cout << "Calculated frequency: " << frequency << " Hz\n";

    LowPassFilter lpf = LowPassFilter(cutoff, sampleRate);
    int totalSamples = sampleRate * duration;
    float* buffer = new float[totalSamples];

    double theta = 2 * M_PI * frequency / sampleRate;
    std::cout << "Theta: " << theta << "\n";
    float temp = 0;

    std::cout << "Generating " << waveType << " wave at " << frequency << " Hz for " << duration << " seconds.\n";
    switch(waveType) {
        case 1:
            std::cout << "Waveform: Sine Wave\n";
            for (int i = 0; i < totalSamples; ++i) {
                buffer[i] = sin(static_cast<double>(i) * theta); // Default to sine wave
            }
            break;
        case 2:
            std::cout << "Waveform: Square Wave\n";
            for (int i = 0; i < totalSamples; ++i) {
                buffer[i] = (sin(i * theta) > 0) ? 1.0f : -1.0f; // Square wave
            }
            break;
        case 3:
            std::cout << "Waveform: Sawtooth Wave\n";
            for (int i = 0; i < totalSamples; ++i) {
                double d_i = static_cast<double>(i);
                buffer[i] = 2.0 * (d_i * frequency / sampleRate - floor(d_i * frequency / sampleRate)) - 1.0; // Sawtooth wave
            }
            break;
        case 4:
            std::cout << "Waveform: Triangle Wave\n";
            for (int i = 0; i < totalSamples; ++i) {
                double d_i = static_cast<double>(i);
                temp = 2.0 * (d_i * frequency / sampleRate - floor(d_i * frequency / sampleRate)) - 1.0; // Sawtooth wave
                buffer[i] = 2.0 * fabs(temp) - 1.0; // Convert to Triangle wave
            }
            break;
        default:
            std::cout << "Unknown waveform type. Defaulting to Sine Wave.\n";
            waveType = 1;
            break;
    }

    // Perform final DSP and formatting for WAV
    drwav_int16* wav_buffer = new drwav_int16[totalSamples];
    for (int i = 0; i < totalSamples; ++i) {
        buffer[i] = lpf.update(buffer[i]);  // Apply low-pass filter to prevent aliasing
        wav_buffer[i] = static_cast<drwav_int16>(buffer[i] * 32767.0f * GAIN); // Scale to 16-bit PCM & apply gain
    }

    std::cout << "Writing to WAV file...\n";
    drwav wav;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = 1;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 16;
    drwav_init_file_write(&wav, "data/recording.wav", &format, NULL);


    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, totalSamples, wav_buffer);
    std::cout << "Wrote " << framesWritten << " frames to recording.wav\n";
    drwav_uninit(&wav);

}

