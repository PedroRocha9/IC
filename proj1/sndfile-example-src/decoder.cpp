#include <iostream>
#include "bit_stream.h"

using namespace std;

int main(int argc, char *argv[]) {
    
    if(argc < 2) {
        cerr << "Usage: " << argv[0] << " <input file>\n";
        return 1;
    }

    BitStream bs(argv[argc-1]);
    cout << bs.readBits(3) << endl;
}
