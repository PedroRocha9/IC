#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
    // print hello world
    // opencv transforms _cif.y4m in FRAMES, because the file has the structure:
    // YUV4MPEG2 W352 H288 Frame rate 30000:1001 (~30 frames per second) 
    // FRAMES
    // ... binary info
    // FRAMES

    VideoCapture cap;

    if(argc == 2){
        cap = VideoCapture(argv[1]);
    } else {
        cap = VideoCapture(0);
    }

    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return 1;
    }

    Mat frame;

    namedWindow("rgb");
    while(true){
        cap >> frame;
        if(frame.empty()){
            break;
        }

        imshow("rgb", frame);

        if(waitKey(30) == 'q'){
            break;
        }
    }

    return 0;
}