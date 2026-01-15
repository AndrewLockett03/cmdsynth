//
// Created by Andrew Lockett on 12/30/25.
//

#define DR_WAV_IMPLEMENTATION
#include "cmdsynth.h"
#include "dr_wav.h"
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {

//    if (argc < 6) {
//        std::cerr << "Usage: " << argv[0] << " <sample_rate> <frequency> <duration> <waveform_type> <cutoff>\n";
//        return 1;
//    }

    float sampleRate; // Sample rate from command line
    std::string frequencyStr;   // Note name from command line
    float duration;   // Duration from command line
    int waveType;       // Waveform type 1-4 from command line
    float cutoff;
    std::vector<drwav_int16> wav_vector;

    std::string flag = argv[1];
    if (flag == "-f") {
        std::ifstream fin;
        fin.open(argv[2]);
        if (!fin.is_open()) {
            std::cerr << "Error opening file: " << argv[2] << "\n";
            return 1;
        }
        fin >> sampleRate >> cutoff >> waveType;
        CmdSynth generator = CmdSynth(sampleRate, cutoff, waveType);
        drwav_int16 *wav_data;
        while (!fin.eof()) {
            fin >> std::ws >> frequencyStr >> duration;
            std::cout << "Generating note " << frequencyStr << " for " << duration << " seconds.\n";
            wav_data = generator.generate_note(frequencyStr, duration);
            wav_vector.insert(wav_vector.end(), wav_data, wav_data + static_cast<size_t>(duration * sampleRate));
            delete[] wav_data; // Clear buffer after every note
        }
        // Short silence at end
        wav_data = generator.generate_note("S", 0.1f);
        wav_vector.insert(wav_vector.end(), wav_data, wav_data + static_cast<size_t>(0.1f * sampleRate));
        // Close file
        fin.close();
    }
    else {
        sampleRate = atof(argv[1]); // Sample rate from command line
        frequencyStr = argv[2];   // Note name from command line
        duration = atof(argv[3]);   // Duration from command line
        waveType = atoi(argv[4]);       // Waveform type 1-4 from command line
        cutoff = atof(argv[5]);   // Cutoff frequency for low-pass filter

        CmdSynth generator = CmdSynth(sampleRate, cutoff, waveType);
        drwav_int16 *wav_data = generator.generate_note(frequencyStr, duration);
        wav_vector.insert(wav_vector.end(), wav_data, wav_data + static_cast<size_t>(duration * sampleRate));
        // Short silence at end
        wav_data = generator.generate_note("S", 0.1f), wav_vector.end();
        wav_vector.insert(wav_vector.end(), wav_data, wav_data + static_cast<size_t>(0.1f * sampleRate));
        delete[] wav_data; // Clear buffer
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


    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, wav_vector.size(), wav_vector.data());
    std::cout << "Wrote " << framesWritten << " frames to recording.wav\n";
    drwav_uninit(&wav);

}
