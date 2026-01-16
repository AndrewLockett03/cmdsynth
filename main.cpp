//
// Created by Andrew Lockett on 12/30/25.
//

#define DR_WAV_IMPLEMENTATION
#include "cmdsynth.h"
#include "dr_wav.h"
#include <iostream>
#include <vector>

int main(int argc, char** argv) {

    if (argc != 5 && argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <sample_rate> <waveform_type> <cutoff> <frequency> <duration> \n"
                  << "Or: " << argv[0] << " <sample_rate> <waveform_type> <cutoff> <input_file>\n"
                  << "Waveform types: 1-Sine, 2-Square, 3-Sawtooth, 4-Triangle\n";
        return 1;
    }

    float sampleRate; // Sample rate from command line
    std::string frequencyStr;   // Note name from command line
    float duration;   // Duration from command line
    int waveType;       // Waveform type 1-4 from command line
    float cutoff;
    std::vector<float> wav_vector;
    std::vector<drwav_int16> final_wav_vector;

    std::string flag = argv[1];
    if (argc == 5) {
        sampleRate = atof(argv[1]); // Sample rate from command line
        waveType = atoi(argv[2]);       // Waveform type 1-4 from command line
        cutoff = atof(argv[3]);   // Cutoff frequency for low-pass filter
        std::string inputFile = argv[4]; // Input file with note sequence

        CmdSynth generator = CmdSynth(sampleRate, cutoff, waveType);
        wav_vector = generator.sequence_notes(inputFile);
        final_wav_vector = generator.float_to_int16(wav_vector);
        wav_vector.clear(); // Clear float buffer
    }
    else {
        sampleRate = atof(argv[1]); // Sample rate from command line
        waveType = atoi(argv[2]);       // Waveform type 1-4 from command line
        cutoff = atof(argv[3]);   // Cutoff frequency for low-pass filter
        frequencyStr = argv[4];   // Note name from command line
        duration = atof(argv[5]);   // Duration from command line

        CmdSynth generator = CmdSynth(sampleRate, cutoff, waveType);
        std::vector<float> buffer_data = generator.generate_single_note(frequencyStr, duration);
        wav_vector.insert(wav_vector.end(), buffer_data.begin(), buffer_data.end());
        buffer_data.clear(); // Clear buffer
        final_wav_vector = generator.float_to_int16(wav_vector);
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


    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, final_wav_vector.size(), final_wav_vector.data());
    std::cout << "Wrote " << framesWritten << " frames to recording.wav\n";
    drwav_uninit(&wav);

}
