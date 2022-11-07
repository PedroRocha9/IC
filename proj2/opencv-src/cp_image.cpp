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

    //loop through the image and copy the pixels to a new image
    Mat new_image = Mat::zeros(image.size(), image.type());
    int channels = image.channels();
    int nRows = image.rows;
    int nCols = image.cols * channels;

    //check if the image is continuous
    if (image.isContinuous()){
        nCols *= nRows;
        nRows = 1;
    }

    //loop through the image and copy the pixels to a new image
    int i,j;
    uchar* pixel;
    for (i = 0; i < nRows; i++){
        pixel = image.ptr<uchar>(i);
        //loop through the columns
        for (j = 0; j < nCols; j++){
            //copy the pixel to the new image
            new_image.at<uchar>(i,j) = pixel[j];
        }
    }

    //write the new image
    imwrite(argv[2], new_image);   

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Time taken: " << elapsed_secs << " ms" << endl;

    return 0;
}
