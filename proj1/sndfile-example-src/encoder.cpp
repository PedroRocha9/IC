#include <iostream>
#include "bit_stream.h"

using namespace std;

int main (int argc, char *argv[]) {	

    //this program converts a ".txt" file that contains only 1's and 0's into a binary file using the bit_stream class

    if (argc < 3) {	
        cerr << "Usage: " << argv [0] << " <input file> <output file>\n" ;
        return 1 ;
    } 

    //open the input file
    ifstream inputFile (argv [1], ios::in) ;
    if (! inputFile) {
        cerr << "Error: could not open input file " << argv [1] << ".\n" ;
        return 1 ;
    }

    string outputFileName = argv[2];

    //the input file is only 1 line containing 1's and 0's
    //save the line to a string variable and print it and its length
    //then close the input file
    string line;
    getline(inputFile, line);
    cout << "Input file: " << line << endl;
    cout << "Input file length: " << line.length() << endl;
    inputFile.close();

    //open the output file
    BitStream outputFile (outputFileName, "w") ;

    //write the bits to the output file
    vector<int> bits;
    for (int i = 0; i < line.length(); i++){
        bits.push_back(line[i] - '0');
    }
    outputFile.writeBits(bits);
    outputFile.close();

    return 0;
} ; 