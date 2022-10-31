#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    //start a timer
    clock_t start = clock();
    
    if (argc < 2) {	
        cerr << "Usage: " << argv [0] << " <input file>\n" ;
        return 1 ;
    }

    // Read the image file
    Mat img = imread(argv[1]);

    int nRows = img.rows;
    int nCols = img.cols * img.channels();

    // check if image is continous
    if (img.isContinuous()) {
        nCols *= img.rows;
        nRows = 1;
    }

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
        }
    }

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Time: " << elapsed_secs << " ms" << endl;

    imshow("Negative Image", img);
    waitKey(0);

    return 0;
}