#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wav_hist.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <wanted_effect> [-g gain default (0.5)] [-nr echo_multiplicity default (2)] <output_file>\n";
		return 1;
	}

	SndfileHandle sfhIn { argv[argc-3] };
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

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, sfhIn.format(),
    sfhIn.channels(), sfhIn.samplerate() };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

    string wanted_effect = argv[argc-2];
    // check if the wanted_effect is single_echo or multiple_echo
    if (wanted_effect != "single_echo" || wanted_effect != "multiple_echo") {
        cerr << "Error: invalid effects\n Usage: wav_effects.cpp <input file> <single_echo [-g gain] [-nr echo_multiplicity] | multiple_echo [-g gain] [-nr echo_multiplicity]> <output_file>\n";
        return 1;
    }
    else if (wanted_effect == "single_echo") {
        int gain = 0.5;
        // check if gain is arg, else set gain to 0.5
        for(int n = 1 ; n < argc ; n++){
            if(string(argv[n]) == "-g") {
                gain = stoi(argv[n+1]);
                break;
            }
            // if there's -nr in single echo : wrong arguments
            else if(string(argv[n]) == "-nr") {
                cerr << "Error: wrong arguments\n Usage of single echo: wav_effects.cpp <input file> single_echo [-g gain] <output_file>\n";
                return 1;
            }
        }
    }
    else if (wanted_effect == "multiple_echo"){
        // check if gain and/or echo_multiplicity are arg, else set to 0.5, 2      
        int gain = 0.5;
        int echo_multiplicity = 2;
        for(int n = 1 ; n < argc ; n++){
            if(string(argv[n]) == "-g") {
                gain = stoi(argv[n+1]);
            }
            else if(string(argv[n]) == "-nr"){
                echo_multiplicity = stoi(argv[n+1]);
            }
        }
    }

    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    size_t nFrames;

    // create a single echo
    if (wanted_effect == "single_echo") {
            sfhOut.writef(&samples[0], nFrames * sfhIn.channels());
    }
    // create a multiple echo
    else if (wanted_effect == "multiple_echo") {
            sfhOut.writef(&samples[0], nFrames * sfhIn.channels());
    }
}