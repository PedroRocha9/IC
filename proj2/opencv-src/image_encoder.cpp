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

//function that calculates the prediction of the next value in the sequence based on 3 values -> pixels to the left, above and to the left top
// if the matrix it's in the first row of the matrix, use only the previous pixel (to the left)
// if it's the first pixel of the matrix, do not use prediction obviously
 

// jpeg type of prediction using YCbCr to YCbCr (opencv converts rgb to YCbCr) 
// when having only 1 line of pixels, predict based on the previous pixel (to the left), like a jpeg encoder
// there's always 3 channels/spaces in the image --> não entendi bem isto xD é Y, Cb e Cr?
// Predict the pixel value based on the pixel value above it, to the left of it and to the left top of it

int main(int argc, char *argv[]){

    auto rgb2ycbcr = [](Mat img) {
        // get the rgb values of the image's matrix and converts them to YCbCr
        for (int i = 0; i < img.rows; i++) {
            for (int j = 0; j < img.cols; j++) {
                cv::Vec3b &pixel = img.at<cv::Vec3b>(i, j);
                //print first pixel
                if(i == 0 && j == 0) {
                    // cout << "pixel: " << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << endl;
                }

                int y = (int) (0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
                int cb = (int) (128 - 0.168736 * pixel[2] - 0.331264 * pixel[1] + 0.5 * pixel[0]);
                int cr = (int) (128 + 0.5 * pixel[2] - 0.418688 * pixel[1] - 0.081312 * pixel[0]);

                pixel[0] = y;
                pixel[1] = cb;
                pixel[2] = cr;
                //print first pixel
                if(i == 0 && j == 0) {
                    // cout << "pixel: " << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << endl;
                }
            }
        }
    };

    //function that calculates average of 3 numbers
    auto average = [](int a, int b, int c) {
        return (a + b + c) / 3;
    };
    //function to calculate m based on u
    auto calc_m = [](int u) {
        //u = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) u / (1 + u)));
    };

    //start a timer
    clock_t start = clock();

    // check if the number of arguments is correct
    if(argc < 4 || argc > 5) {
        cerr << "Usage: " << argv[0] << " <input file> <output file> <m (must not use auto!) | bs (must use auto!)> [auto]\n";
        return 1;
    }
    
    //check if "auto" is passed in
    bool autoMode = false;
    if (argc == 5) {
        if (strcmp(argv[4], "auto") == 0) {
            autoMode = true;
        }
    }
    short m = atoi(argv[3]);
    int bs = m;

    // read the image
    Mat img = imread(argv[1]);
    // check if the image is loaded
    if (img.empty()){
        // cout << "Could not load image: " << argv[1] << endl;
        return -1;
    }

    // convert the image to YCbCr
    rgb2ycbcr(img);

    //function that predicts the next value in the sequence based on 3 previous values
    auto predict = [](int a, int b, int c) {
        //3*a - 3*b + c
        return 3*a - 3*b + c;
    };

    vector<int> m_vector;
    vector<int> valuesToBeEncoded;
    
    //print the first pixel of the img
    // cout << "First pixel: " << img.at<cv::Vec3b>(0, 0) << endl;

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
    
    //end the timer
    clock_t end = clock();

    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    // cout << "Execution time: " << elapsed_secs << " ms" << endl;
 
    return 0;
}

