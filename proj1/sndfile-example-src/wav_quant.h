#ifndef WAVQUANT_H
#define WAVQUANT_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>


class WAVQuant {
  private:
    std::vector<short> quant_samples;


  public:
	WAVQuant(const SndfileHandle& sfh) {
		quant_samples.resize(sfh.channels());
	}

	void quant(const std::vector<short>& samples, size_t num_bits_to_cut) {
        short tmp = 0;
		for (auto sample : samples) {
            //take each short sample and turn into 0 the num_bits_to_cut least significant bits
            sample = sample >> num_bits_to_cut;
            //shift the sample back to its original position
            tmp = sample << num_bits_to_cut;
            //add the sample to the vector
            quant_samples.push_back(tmp);
        }
	}

	void toFile(SndfileHandle sfhOut) const {
        sfhOut.write(quant_samples.data(), quant_samples.size());
		
	}
};

#endif

