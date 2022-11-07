#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
 
using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
    // start a timer
    clock_t start = clock();
    
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <image file> <output file> <degrees_to_rotate> [view]" << endl;
        return 1;
    }

    // reading image file
    Mat img = imread(argv[1]);
    if (img.empty()) {
        cerr << "Could not open or find the image!" << endl; ;
        cerr << "Usage: " << argv[0] << " <image file> <output file> <degrees_to_rotate> [view]" << endl;
        return -1;
    }

    // verify that degrees_to_rotate is a multiple of 90
    int degrees_to_rotate = atoi(argv[3]);
    if (atoi(argv[3]) % 90 != 0) {
        cerr << "Degrees to rotate must be a multiple of 90" << endl;
        return 1;
    }

    // rotate the image
    Mat rotated_image;
    Point2f center(img.cols/2.0, img.rows/2.0);
    Mat rot = getRotationMatrix2D(center, degrees_to_rotate, 1.0);
    warpAffine(img, rotated_image, rot, img.size());
    imwrite(argv[2], rotated_image);

    // end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    // print the time in miliseconds
    cout << "Time: " << elapsed_secs*1000 << " ms" << endl;

    try {
        if (string(argv[4]) == "view") {
            imshow("Rotated Image", rotated_image);
            waitKey(0);
        }
    } catch (exception& e) {
        return 0;
    }

    return 0;
}
