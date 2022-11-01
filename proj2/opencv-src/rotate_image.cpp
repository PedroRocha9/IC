#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
 
using namespace cv;
using namespace std;

int main(int argc, char *argv[]){
    // start a timer
    clock_t start = clock();
    
    if (argc < 3) {
        cout << "it entered lmao" << endl;
        cerr << "Usage: " << argv[0] << " <image_file> <degrees_to_rotate>" << endl;
        return 1;
    }

    // verify that degrees_to_rotate is a multiple of 90
    int degrees_to_rotate = atoi(argv[2]);
    if (atoi(argv[2]) % 90 != 0) {
        cerr << "Degrees to rotate must be a multiple of 90" << endl;
        return 1;
    }

    //reading image file
    Mat image = imread(argv[1]);
    if (!image.data) {
        cerr << "Could not open or find the image" << endl;
        return 1;
    }

    // rotate the image
    Mat rotated_image;
    Point2f center(image.cols/2.0, image.rows/2.0);
    Mat rot = getRotationMatrix2D(center, degrees_to_rotate, 1.0);
    warpAffine(image, rotated_image, rot, image.size());

    // end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    // print the time in miliseconds
    cout << "Time: " << elapsed_secs*1000 << " ms" << endl;

    imshow("Rotated Image", rotated_image);
    waitKey(0);

    return 0;
}