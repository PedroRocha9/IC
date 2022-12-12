#include "golomb.h"
#include "bitStream.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include "YUV4MPEG2Reader.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){
    //This program will encode a video file in the format of YUV4MPEG2 into a binary file using intraframe encoding

    //Check for correct number of arguments
    // if(argc != 4){
    //     cout << "Usage: " << argv[0] << " <input file> <output file> <block size>" << endl;
    //     return 1;
    // }

    //Open input file
    // FILE* input = fopen(argv[1], "r");
    // if(input == NULL){
    //     cout << "Error: Could not open input file" << endl;
    //     return 1;
    // }

    // int blockSize = atoi(argv[3]);
    

    // //read first line (until newline
    // char* line = NULL;
    // size_t len = 0;
    // getline(&line, &len, input);

    // //extract width and height
    // int width, height;
    // sscanf(line, "YUV4MPEG2 W%d H%d", &width, &height);

    // string colorspace;

    // //check if 'C' is in the line
    // if(strchr(line, 'C') == NULL){
    //     colorspace = "420";
    // }

    YUV4MPEG2Reader reader(argv[1]);

    cout << reader.width() << endl;
    cout << reader.height() << endl;
    cout << reader.colorSpace() << endl;

    return 0;
}