#include "golomb.h"
#include <iostream>
#include <string>
#include <vector>
#include <fftw3.h>
#include <sndfile.hh>
#include "bitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

    //start a timer
    clock_t start = clock();

    if(argc < 4) {
		cerr << "Usage: " << argv[0] << " <input file> <output file> <m>\n";
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

    string output = argv[2];

    short m = atoi(argv[argc-1]);

    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());

    //read the wav file and store the samples in a vector
    vector<short> samplesVector;
    while(true) {
        auto framesRead = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE);
        if(framesRead == 0) {
            break;
        }
        for(int i = 0; i < framesRead * sfhIn.channels(); i++) {
            samplesVector.push_back(samples[i]);
        }
    }

    //create a vector that will hold the values to be encoded
    //these values are calculated based on predictive coding
    //starting from the fourth sample, the value to be added to the vector
    //is the difference between the current sample and the following expression:
    //current sample - (3*previous sample - 3*previous previous sample + previous previous previous sample)
    vector<int> valuesToBeEncoded;
    for(int i = 0; i < 3; i++) {
        valuesToBeEncoded.push_back(samplesVector[i]);
    }
    for(int i = 3; i < samplesVector.size(); i++) {
        valuesToBeEncoded.push_back(samplesVector[i] - (3*samplesVector[i-1] - 3*samplesVector[i-2] + samplesVector[i-3]));
    }

    //print the average of the values to be encoded
    int sum = 0;
    for(int i = 0; i < valuesToBeEncoded.size(); i++) {
        sum += valuesToBeEncoded[i];
    }
    double average = (double)sum / valuesToBeEncoded.size();
    cout << "Average: " << average << endl;

    string encodedString = "";
    Golomb golomb(m);
    for(int i = 0; i < valuesToBeEncoded.size() ; i++) {
        encodedString += golomb.intToBitString(valuesToBeEncoded[i]);
    }

    BitStream bitStream(output, "w");

    vector<int> bits;
    for(int i = 0; i < encodedString.length(); i++) {
        bits.push_back(encodedString[i] - '0');
    }
    bitStream.writeBits(bits);
    bitStream.close();

    

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;
    


    return 0;
}