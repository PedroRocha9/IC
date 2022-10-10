#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file1> <input file2>\\n";
		return 1;
	}

	SndfileHandle sfhIn1 { argv[argc-2] };
	if(sfhIn1.error()) {
		cerr << "Error: invalid input file1\n";
		return 1;
    }

    SndfileHandle sfhIn2 { argv[argc-1] };
	if(sfhIn2.error()) {
		cerr << "Error: invalid input file2\n";
		return 1;
    }

    // Check file 1 format
	if((sfhIn1.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file1 is not in WAV format\n";
		return 1;
	}
	if((sfhIn1.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file1 is not in PCM_16 format\n";
		return 1;
	}

    // Check file 2 format
    if((sfhIn2.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file2 is not in WAV format\n";
		return 1;
	}

	if((sfhIn2.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file2 is not in PCM_16 format\n";
		return 1;
	}

    cout << "file 1 " << sfhIn1.frames() << endl;
    cout << "file 2 " << sfhIn2.frames() << endl;
    // Check if files have the same number of frames
    if(sfhIn1.frames() != sfhIn2.frames()) {
        cerr << "Error: files have different number of frames\n";
        return 1;
    }

    vector<short> samples_f1(FRAMES_BUFFER_SIZE * sfhIn1.channels());
    vector<short> samples_f2(FRAMES_BUFFER_SIZE * sfhIn2.channels());

    size_t nFrames;
    double energy_signal { 0 };
    double energy_noise { 0 };
    double snr;

    while((nFrames = sfhIn1.readf(samples_f1.data(), FRAMES_BUFFER_SIZE))) {
        sfhIn2.readf(samples_f2.data(), FRAMES_BUFFER_SIZE);

        samples_f1.resize(nFrames * sfhIn1.channels());
        samples_f2.resize(nFrames * sfhIn2.channels());

        for (long unsigned int i = 0; i < samples_f1.size(); i++) {
            energy_signal += abs(samples_f1[i]);
            energy_noise += abs(samples_f1[i] - samples_f2[i]);
        }    
    }

    snr = 10 * log10(energy_signal / energy_noise);
    cout << "SNR: " << snr << " dB" << endl;
}
