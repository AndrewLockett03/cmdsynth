//
// Created by Andrew Lockett on 12/30/25.
//

#define DR_WAV_IMPLEMENTATION
#include "cmdsynth.h"
#include "dr_wav.h"
#include <iostream>

int main(int argc, char** argv) {

    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <sample_rate> <frequency> <duration> <waveform_type> <cutoff>\n";
        return 1;
    }

    float sampleRate = atof(argv[1]); // Sample rate from command line
    char* frequencyStr = argv[2];   // Note name from command line
    float duration = atof(argv[3]);   // Duration from command line
    int waveType = atoi(argv[4]);       // Waveform type 1-4 from command line
    float cutoff = atof(argv[5]);   // Cutoff frequency for low-pass filter

    CmdSynth generator = CmdSynth(sampleRate, cutoff, waveType);
    drwav_int16* wav_data = generator.generate_note(frequencyStr, duration);
    int length = duration * sampleRate;

    std::cout << "Writing to WAV file...\n";
    drwav wav;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = 1;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 16;
    drwav_init_file_write(&wav, "data/recording.wav", &format, NULL);


    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, length, wav_data);
    std::cout << "Wrote " << framesWritten << " frames to recording.wav\n";
    drwav_uninit(&wav);

}
