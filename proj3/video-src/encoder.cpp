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

    YUV4MPEG2Reader reader(argv[1]);

    cout << reader.width() << endl;
    cout << reader.height() << endl;
    cout << reader.colorSpace() << endl;

    return 0;
}