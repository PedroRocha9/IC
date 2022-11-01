#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    //start a timer
    clock_t start = clock();
    
    // Number value 
    if (argc < 4) {	
        cerr << "Usage: " << argv [0] << " <input file> <output file> <num> [view]" << endl;
        return 1 ;
    }

    Mat img = imread(argv[1]);
    
    if (img.empty()) {
        cerr << "Could not open or find the image!" << endl; ;
        cerr << "Usage: " << argv [0] << " <input file> <output file> <num> [view]" << endl;
        return -1;
    }

    int b = 0;
    
    try {
        b = stoi(argv[3]);

        if (b < -255 || b > 255) {
            cerr << "num value must be between -255 and 255\n";
            return 1;
        }  
    } catch (exception& e) {
        cerr << "argument must be a number\n";
        return 1;
    }

    Mat output = img + Scalar::all(b);

    imwrite(argv[2], output);

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Time: " << elapsed_secs << " ms" << endl;
    
    try {
        if (string(argv[4]) == "view") {
            imshow("Transformed Image", output);
            waitKey(0);
        }
    } catch (exception& e) {
        return 0;
    }

    return 0;
}
