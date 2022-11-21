#include "golomb.h"
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fftw3.h>
#include <sndfile.hh>
#include "bitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

    //function that calculates average of 3 numbers
    auto average = [](int a, int b, int c) {
        return (a + b + c) / 3;
    };

    //function that predicts the next value in the sequence based on 3 previous values
    auto predict = [](int a, int b, int c) {
        //3*a - 3*b + c
        return 3*a - 3*b + c;
    };

    //function to calculate m based on u
    auto calc_m = [](int u) {
        //u = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) u / (1 + u)));
    };

    //start a timer
    clock_t start = clock();

    if(argc != 4 && argc != 5) {
		cerr << "Usage: " << argv[0] << " <input file> <output file> <m | bs> [auto] \n";
		return 1;
	}

    //check if "-auto" is passed in
    bool autoMode = false;
    if (argc == 5) {
        if (strcmp(argv[4], "auto") == 0) {
            autoMode = true;
        }
    }

    if (!autoMode) {
        if (argc != 4) {
            cerr << "Usage: " << argv[0] << " <input file> <output file> <m | bs> [auto] \n";
            return 1;
        }
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

    short m = atoi(argv[3]);
    short bs = m;
    short og = m;

    size_t nFrames { static_cast<size_t>(sfhIn.frames()) };
    size_t nChannels { static_cast<size_t>(sfhIn.channels()) };

    vector<short> samples(nChannels * nFrames);

	sfhIn.readf(samples.data(), nFrames);

    vector<short> left_samples(nFrames);
    vector<short> right_samples(nFrames);

    if (nChannels > 1){
        //split samples into left and right channels
        for (int i = 0; i < nFrames; i++) {
            left_samples[i] = samples[i * nChannels];
            right_samples[i] = samples[i * nChannels + 1];
        }
    }

	size_t nBlocks { static_cast<size_t>(ceil(static_cast<double>(nFrames) / bs)) };

	// Do zero padding, if necessary
	samples.resize(nBlocks * bs * nChannels);

    //zero padding of left and right channels
    left_samples.resize(nBlocks * bs);
    right_samples.resize(nBlocks * bs);

    vector<int> m_vector;

    vector<int> valuesToBeEncoded;
    if(nChannels < 2){
        for(int i = 0; i < samples.size(); i++) {
            if (i >= 3) {
                int difference = samples[i] - predict(samples[i-1], samples[i-2], samples[i-3]);
                valuesToBeEncoded.push_back(difference);
            }
            
            else valuesToBeEncoded.push_back(samples[i]);
        }
    }
    else{
        for(int i = 0; i < nFrames; i++){
            if (i >= 3) {
                int difference = left_samples[i] - predict(left_samples[i-1], left_samples[i-2], left_samples[i-3]);
                valuesToBeEncoded.push_back(difference);
            }
            
            else valuesToBeEncoded.push_back(left_samples[i]);

            //calculate m every bs samples

            if (i % bs == 0 && i != 0) {
                int sum = 0;
                for (int j = i-bs; j < i; j++) {
                    sum += abs(valuesToBeEncoded[j]);
                }
                int u = round(sum/bs);
                m = calc_m(u);
                
                m_vector.push_back(m);
            }
        }

        for(int i = 0; i < nFrames; i++){
            if (i >= 3) {
                int difference = right_samples[i] - predict(right_samples[i-1], right_samples[i-2], right_samples[i-3]);
                valuesToBeEncoded.push_back(difference);
            }
            
            else valuesToBeEncoded.push_back(right_samples[i]);

            //calculate m every bs samples

            if (i % bs == 0 && i != 0) {
                int sum = 0;
                for (int j = i-bs; j < i; j++) {
                    sum += abs(valuesToBeEncoded[j]);
                }
                int u = round(sum/bs);
                m = calc_m(u);



                
                m_vector.push_back(m);
            }
        }
    }

    // //encode the values

    
    string encodedString = "";
    Golomb g;

    if (!autoMode){
        for(int i = 0; i < valuesToBeEncoded.size() ; i++) {
            encodedString += g.encode(valuesToBeEncoded[i], og);
        }
    }
    else{
        int m_index = 0;
        for (int i = 0; i < valuesToBeEncoded.size(); i++) {
            if (i % bs == 0 && i != 0) {
                m_index++;
                if (m_index >= m_vector.size()) {
                    break;
                }
            }
            encodedString += g.encode(valuesToBeEncoded[i], m_vector[m_index]);       
        }
        // cout << "encoded string size: " << encodedString.size() << endl;
    }


    //write the encoded string to a file
    BitStream bitStream(output, "w");

    vector<int> bits;
    vector<int> encoded_bits;

    //the next bits will be the encoded string

    for(int i = 0; i < encodedString.length(); i++) {
        encoded_bits.push_back(encodedString[i] - '0');
    }
    //if the encoded string is not a multiple of 8, add 0s to the end of the string
    //until it is a multiple of 8
    int count_zeros = 0;
    while (encoded_bits.size() % 8 != 0) {
        encoded_bits.push_back(0);
        count_zeros++;
    }




    //the first 16 bits of the file are the number of channels
    for(int i = 15; i >= 0; i--) {
        bits.push_back((sfhIn.channels() >> i) & 1);
    }
    // cout << "channels: " << sfhIn.channels() << endl;

    //the next 32 bits of the file are the number of frames
    for(int i = 31; i >= 0; i--) {
        bits.push_back((nFrames >> i) & 1);
    }
    // cout << "nFrames: " << nFrames << endl;

    //the next 16 bits is the block size
    for(int i = 15; i >= 0; i--) {
        bits.push_back((bs >> i) & 1);
    }
    // cout << "block size: " << bs << endl;

    //the next 16 bits is the number of zeros added to the end of the encoded string
    for(int i = 15; i >= 0; i--) {
        bits.push_back((count_zeros >> i) & 1);
    }
    // cout << "number of zeros added: " << count_zeros << endl;

    //the next 16 bits of the output file will be size of m_vector converted to binary
    for (int i = 15; i >= 0; i--){
        bits.push_back((m_vector.size() >> i) & 1);
    }
    // cout << "m_size: " << m_vector.size() << endl;

    // the next bits will be the values of m_vector converted to binary
    for (int i = 0; i < m_vector.size(); i++) {
        for (int j = 15; j >= 0; j--) {
            bits.push_back((m_vector[i] >> j) & 1);
        }
    }
    
    //the next bits are the encoded bits
    for (int i = 0; i < encoded_bits.size(); i++) {
        bits.push_back(encoded_bits[i]);
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