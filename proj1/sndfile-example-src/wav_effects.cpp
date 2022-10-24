#include <iostream>
#include <vector>
#include <math.h>
#include <cmath>
#include <sndfile.hh>
#include "wav_hist.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

	if(argc < 4) {
		cerr << "Usage: " << argv[0] << " <input file> <output_file> <wanted_effect> <gain> <delay>\n";
		return 1;
	}

	SndfileHandle sfhIn { argv[1] };
	if(sfhIn.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    }

	if((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}

    SndfileHandle sfhOut { argv[2], SFM_WRITE, sfhIn.format(),
    sfhIn.channels(), sfhIn.samplerate() };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

    string wanted_effect = argv[3];
    // Check if the wanted_effect is single_echo or multiple_echo
    float gain = 0.8;
    int delay = 44100;

    if (wanted_effect != "single_echo" && wanted_effect != "multiple_echo" && wanted_effect != "amplitude_modulation" && wanted_effect != "reverse") {
        cerr << "Error: invalid effects\nUsage: wav_effects.cpp <input file> <output_file> <wanted_effect> <gain> <delay in Hertz>\n";
        return 1;
    }
    else if (wanted_effect == "single_echo" || wanted_effect == "multiple_echo") {
        // Check if the gain, delay and echo_multiplicity are valid
        try {
            gain = stof(argv[argc-2]);
            delay = stoi(argv[argc-1]);
        } catch(exception &err) {
            cerr << "Error: invalid gain or delay\n";
            return 1;
        }
    }
    else if ((wanted_effect == "amplitude_modulation" || wanted_effect == "reverse") && argc > 4){
        cerr << "Error: invalid arguments\nUsage: wav_effects.cpp <input file> <output_file> <amplitude_modulation || reverse>\n";
        return 1;
    }

    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> samples_out;
    samples_out.resize(0);
    size_t nFrames;
    short sample_out;

    // Create an echo
    if (wanted_effect == "single_echo" || wanted_effect == "multiple_echo") {
        while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
            samples.resize(nFrames * sfhIn.channels());
            
            for (int i = 0; i < (int)samples.size(); i++) {
                if (wanted_effect == "single_echo") {
                    // y(n) = x(n) + a * x(n-delay)
                    if (i >= delay) {      
                        sample_out = (samples.at(i) + gain * samples.at(i - delay))/(1 + gain);
                    } else {
                        sample_out = samples.at(i);
                    }
                } else if (wanted_effect == "multiple_echo") {
                    // y(n) = x(n) + a * y(n-delay)
                    if (i >= 1) {
                        sample_out = (samples.at(i) + gain * samples_out.at(i - 1))/(1 + gain);
                    } else {
                        sample_out = samples.at(i);
                    }
                }               

                samples_out.insert(samples_out.end(), sample_out);
            }
        }
    } else if (wanted_effect == "amplitude_modulation") {
        // y(n) = x(n) * cos(2*pi*(f/fa)*n)
        while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
            samples.resize(nFrames * sfhIn.channels());

            for (int i = 0; i < (int)samples.size(); i++) {
                sample_out = samples.at(i) * cos(2 * M_PI * (1.0/sfhIn.samplerate()) * i);
                samples_out.insert(samples_out.end(), sample_out);
            }
        }
    } else if(wanted_effect == "reverse") {
        while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
            samples.resize(nFrames * sfhIn.channels());

            for (int i = (int)samples.size() - 1; i >= 0; i--) samples_out.insert(samples_out.end(), samples.at(i));
        }        
    }

    sfhOut.writef(samples_out.data(), samples_out.size() / sfhIn.channels());
}
