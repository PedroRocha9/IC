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

// function to get the rgb values of the image's matrix and converts them to YCvCr
void rgb2ycbcr(cv::Mat &image) {
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            cv::Vec3b &pixel = image.at<cv::Vec3b>(i, j);
            int r = pixel[2];
            int g = pixel[1];
            int b = pixel[0];
            int y = (int) (0.299 * r + 0.587 * g + 0.114 * b);
            int cb = (int) (128 - 0.168736 * r - 0.331264 * g + 0.5 * b);
            int cr = (int) (128 + 0.5 * r - 0.418688 * g - 0.081312 * b);
            pixel[0] = y;
            pixel[1] = cb;
            pixel[2] = cr;
        }
    }
}

// function that calculates the average of 3 numbers
int average(int a, int b, int c) {
    return (a + b + c) / 3;
}

//function that calculates the prediction of the next value in the sequence based on 3 values -> pixels to the left, above and to the left top
// if the matrix it's in the first row of the matrix, use only the previous pixel (to the left)
// if it's the first pixel of the matrix, do not use prediction obviously
int predict(){
    return 0;
}
 

// jpeg type of prediction using YCvCr to YCvCr (opencv converts rgb to YCvCr) 
// when having only 1 line of pixels, predict based on the previous pixel (to the left), like a jpeg encoder
// there's always 3 channels/spaces in the image --> não entendi bem isto xD é Y, Cb e Cr?
// Predict the pixel value based on the pixel value above it, to the left of it and to the left top of it

int main(int argc, char *argv[]){
    return 0;
}

