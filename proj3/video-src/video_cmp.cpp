// lossy version of the video codec based on Golomb coding of the prediction residuals
// i.e. by simply quantization of those residuals

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include "YUV4MPEG2Reader.h"
#include "golomb.h"
#include "bitStream.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

// this is a program to help assessing the quality of the lossy compressed videos sequence
// it compares 2 video sequences in terms of the peak signal to noise ratio (PSNR)
// with the expression PSNR = 10 * log10(A^2 / e^2)
// where A is the maximum value of the pixel (255 for 8-bit images)
// and e^2 is the mean squared error between the reconstructed, f~, and the original frames, f

// e^2 = 1/(M*N) * sum_r_to_N(sum_c_M((f-f~)^2))
// where N and M are the number of rows and columns of the video frames

int main(int argc, char *argv[]) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " input_file1 input_file2" << endl;
        return 1;
    }

    YUV4MPEG2Reader video1(argv[1]);
    YUV4MPEG2Reader video2(argv[2]);

    // check if the two videos have the same size
    if (video1.width() != video2.width() || video1.height() != video2.height()) {
        cout << "The two videos must have the same dimensions!" << endl;
        cout << "Video 1 has: " << video1.width() << "x" << video1.height() << endl;
        cout << "Video 2 has: " << video2.width() << "x" << video2.height() << endl;
        return 1;
    }

    // get the number of frames
    int num_frames1 = video1.numFrames();
    int num_frames2 = video2.numFrames();

    // check if the two videos have the same number of frames
    if (num_frames1 != num_frames2) {
        cout << "The two videos have different number of frames" << endl;
        return 1;
    }

    // vector for PSNR values
    vector<double> psnr_values;

    // loop through the frames of the two videos
    for (int i = 0; i < num_frames1; i++) {
        // get the current frame of the first video
        Mat frame1 = video1.next_frame();
        // get the current frame of the second video
        Mat frame2 = video2.next_frame();

        // create a matrix to store the difference between the two frames
        Mat diff = Mat::zeros(height, width, CV_8UC1);

        // loop through the rows of the frame
        for (int r = 0; r < height; r++) {
            // loop through the columns of the frame
            for (int c = 0; c < width; c++) {
                // get the pixel value of the first frame
                int pixel1 = frame1.at<uchar>(r, c);
                // get the pixel value of the second frame
                int pixel2 = frame2.at<uchar>(r, c);

                // calculate the difference
                int diff_pixel = pixel1 - pixel2;

                // store the difference in the diff matrix
                diff.at<uchar>(r, c) = diff_pixel;
            }
        }
    }
}


