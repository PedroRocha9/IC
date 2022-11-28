#include "golomb.h"
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "bitStream.h"

using namespace std;
using namespace cv;

// function that calculates the average of 3 numbers
int average(int a, int b, int c) {
    return (a + b + c) / 3;
}

//function that calculates the prediction of the next value in the sequence based on 3 values -> pixels to the left, above and to the left top
// if the matrix it's in the first row of the matrix, use only the previous pixel (to the left)
// if it's the first pixel of the matrix, do not use prediction obviously
 

// jpeg type of prediction using YCbCr to YCbCr (opencv converts rgb to YCbCr) 
// when having only 1 line of pixels, predict based on the previous pixel (to the left), like a jpeg encoder
// there's always 3 channels/spaces in the image --> não entendi bem isto xD é Y, Cb e Cr?
// Predict the pixel value based on the pixel value above it, to the left of it and to the left top of it

int main(int argc, char *argv[]){

     //start a timer
    clock_t start = clock();

    // check if the number of arguments is correct
    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
        return 1;
    }

    // read the image
    Mat img = imread(argv[1]);
    // check if the image is loaded
    if (img.empty()){
        cout << "Could not load image: " << argv[1] << endl;
        return -1;
    }

    // get the rgb values of the image's matrix and converts them to YCbCr
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            cv::Vec3b &pixel = img.at<cv::Vec3b>(i, j);

            int y = (int) (0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
            int cb = (int) (128 - 0.168736 * pixel[2] - 0.331264 * pixel[1] + 0.5 * pixel[0]);
            int cr = (int) (128 + 0.5 * pixel[2] - 0.418688 * pixel[1] - 0.081312 * pixel[0]);

            pixel[0] = y;
            pixel[1] = cb;
            pixel[2] = cr;
            
            // print the first row of pixels with the YCbCr values
            cout << "Y: " << y << " Cb: " << cb << " Cr: " << cr << endl;
        }
    }

    //function that calculates average of 3 numbers
    auto average = [](int a, int b, int c) {
        return (a + b + c) / 3;
    };

    //function that predicts the next value in the sequence based on 3 previous values
    auto predict = [](int a, int b, int c) {
        //3*a - 3*b + c
        return 3*a - 3*b + c;
    };

    vector<int> m_vector;
    vector<int> valuesToBeEncoded;
    
    //print the first pixel of the img
    cout << "First pixel: " << img.at<cv::Vec3b>(0, 0) << endl;

    if (img.rows == 1){   
        for(int i = 0; i < img.rows; i++) {
            for(int j=0; j < img.cols; j++){
                //int prediction = predict(img[i,j], 0, 0);
                // int m = m - prediction;
                // m_vector.push_back(m);
                // valuesToBeEncoded.push_back(m);
            }
        }
    }
    else {
    }
    
    //end the timer
    clock_t end = clock();

    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;
 
    return 0;
}

