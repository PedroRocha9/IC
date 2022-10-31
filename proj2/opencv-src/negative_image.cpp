#include "opencv2/highgui/highgui.hpp"
#include <iostream>


int main(int argc, char** argv) {
    //start a timer
    clock_t start = clock();

    cv::Mat invert_image(cv::Mat const& input)
    {
        return 255 - input;
    }

    return 0;
}