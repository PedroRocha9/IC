#include <iostream>
#include <vector>
#include <cmath>
#include <fftw3.h>
#include <sndfile.hh>
#include "bit_stream.h"

using namespace std;

int main (int argc, char *argv[])
{

    if(argc < 3 || argc > 3 ){
		cerr << "Usage: " << argv[0] << " <input file (binary)> <output file>\n";
		return 1;
	}

    string inputFileName = argv[1];

    BitStream bitStream(inputFileName, "r");

    vector<int> v_bs = bitStream.readBits(16);
    vector<int> v_nBlocks= bitStream.readBits(16);
    vector<int> v_nChannels = bitStream.readBits(16);
    vector<int> v_sampleRate = bitStream.readBits(16);
    vector<int> v_nFrames = bitStream.readBits(32);

    //convert vector<int> to int
    int bs = 0;
    for(int i = 0; i < v_bs.size(); i++) {
        bs += v_bs[i] * pow(2, v_bs.size() - i - 1);
    }

    int nBlocks = 0;
    for(int i = 0; i < v_nBlocks.size(); i++) {
        nBlocks += v_nBlocks[i] * pow(2, v_nBlocks.size() - i - 1);
    }

    int nChannels = 0;
    for(int i = 0; i < v_nChannels.size(); i++) {
        nChannels += v_nChannels[i] * pow(2, v_nChannels.size() - i - 1);
    }


    int sampleRate = 0;
    for(int i = 0; i < v_sampleRate.size(); i++) {
        sampleRate += v_sampleRate[i] * pow(2, v_sampleRate.size() - i - 1);
    }

    int nFrames = 0;
    for(int i = 0; i < v_nFrames.size(); i++) {
        nFrames += v_nFrames[i] * pow(2, v_nFrames.size() - i - 1);
    }

    // cout << "bs: " << bs << endl;
    // cout << "nBlocks: " << nBlocks << endl;
    //cout << "nChannels: " << nChannels << endl;
    // cout << "sampleRate: " << sampleRate << endl;
    //cout << "nFrames: " << nFrames << endl;

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, nChannels, sampleRate };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

    int total = bitStream.getFileSize() - 12;
    long totalBits = total*8;

    // cout << "totalBits: " << totalBits << endl;

    vector<int> x_dct_bits = bitStream.readBits(totalBits);
    vector<vector<double>> x_dct(nChannels, vector<double>(nBlocks * bs));
    vector<int> tmp;

    
    for(int i = 0; i < x_dct_bits.size(); i+=32) {
        //each 32 bits is a int (signed)
        int temp = 0;
        for(int j = 0; j < 32; j++) {
            temp += x_dct_bits[i+j] * pow(2, 31 - j);
        }
        tmp.push_back(temp);
    }

    bitStream.close();

    //x_dct is a array of vector<int> turned into a vector of ints
    //the conversion was made to make it easier to read the bits from the file
    //the original loop was:
    //vector<int>tmp;
    // for(int n = 0; i < nBlocks; i++) {
        //for(int c = 0; c < nChannels; c++) {
            //for (int k = 0; k < bs; k++) {
                //tmp.push_back(x_dct[c][n*bs + k]);

    //in this file, we read the bits from the file and convert them to ints, wich is our tmp vector
    //now, we need to convert tmp to x_dct (vector<vector<int>>) again keeping the same order
    //the conversion is made in the following loop
    int count = 0;
    for(int n = 0; n < nBlocks; n++) {
        for(int c = 0; c < nChannels; c++) {
            for (int k = 0; k < bs; k++) {
                x_dct[c][n*bs + k] = tmp[count];
                count++;
            }
        }
    }

    vector<double> x(bs);
    vector<short> samples(nChannels * nFrames);
    samples.resize(nBlocks * bs * nChannels);
    
    // Inverse DCT
	fftw_plan plan_i = fftw_plan_r2r_1d(bs, x.data(), x.data(), FFTW_REDFT01, FFTW_ESTIMATE);
	for(size_t n = 0 ; n < nBlocks ; n++)
		for(size_t c = 0 ; c < nChannels ; c++) {
			for(size_t k = 0 ; k < bs ; k++)
				x[k] = x_dct[c][n * bs + k];

			fftw_execute(plan_i);
			for(size_t k = 0 ; k < bs ; k++)
				samples[(n * bs + k) * nChannels + c] = static_cast<short>(round(x[k]));

		}

    //print samples
    // for (int i = 0; i < samples.size(); i++) {
    //     cout << samples[i] << endl;
    // }

    
    
    sfhOut.writef(samples.data(), nFrames);
    return 0;

}
