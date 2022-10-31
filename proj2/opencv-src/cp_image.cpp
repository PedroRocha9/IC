#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv ) {

    //start a timer 
    clock_t start = clock();

    if (argc != 3){
        cout << "Usage: "<< argv[0] <<" <input_image> <output_image>" << endl;
        return -1;
    }

    //read the image
    Mat image = imread(argv[1]);

    //check if the image is loaded
    if (image.empty()){
        cout << "Could not load image: " << argv[1] << endl;
        return -1;
    }

    //duplicate the image and save it
    Mat image2 = image.clone();
    imwrite(argv[2], image2);


    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;

    return 0;
}
