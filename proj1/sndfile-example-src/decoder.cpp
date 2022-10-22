#include <iostream>
#include "bit_stream.h"

using namespace std;

int main (int argc, char *argv[]) {	

    //this program converts a binary file and converts it to a ".txt" file that contains only 1's and 0's using the bit_stream class

    if (argc < 3) {	
        cerr << "Usage: " << argv [0] << " <input file> <output file>\n" ;
        return 1 ;
    }

    //open the input file
    BitStream inputFile (argv [1], "r") ;
    
    //open the output file
    ofstream outputFile (argv [2], ios::out) ;
    if (! outputFile) {
        cerr << "Error: could not open output file " << argv [2] << ".\n" ;
        return 1 ;
    }

    //read the bits from the input file
    vector<int> bits;
    bits = inputFile.readBits(inputFile.getFileSize() * 8);
    inputFile.close();

    //write the bits to the output file
    for (int i = 0; i < bits.size(); i++){
        outputFile << bits[i];
    }
    outputFile.close();

    return 0;
}