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
        // convert to YCbCr
        Mat ycbcr;
        cvtColor(img, ycbcr, COLOR_BGR2YCrCb);
        return ycbcr;
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
        cerr << "Usage: " << argv[0] << " <input file> <output file> <m(must not use auto!)> [auto]\n";
        return 1;
    }
    
    //check if "auto" is passed in
    bool autoMode = false;
    short m = 0;
    int bs = 0;
    short original = 0;
    string output = argv[2];
    // read the image
    Mat img = imread(argv[1]);
    if (argc == 5) {
        if (strcmp(argv[4], "auto") == 0) {
            autoMode = true;
        }
        m = atoi(argv[3]);
        original = m;
        bs = m;

    } else if (argc == 4) {
        if (strcmp(argv[3], "auto") == 0) {
            autoMode = true;
        }
        //bs = half the number of columns of the image
        bs = img.cols;
    }
    
    // check if the image is loaded
    if (img.empty()){
        cout << "Could not load image: " << argv[1] << endl;
        return -1;
    }

    //save image type
    int type = img.type();

     // convert the image to YCbCr
    img = rgb2ycbcr(img);

    vector<int> Ym_vector;
    vector<int> Cbm_vector;
    vector<int> Crm_vector;
    vector<int> YvaluesToBeEncoded;
    vector<int> CbvaluesToBeEncoded;
    vector<int> CrvaluesToBeEncoded;
    
    int pixel_index = 0;

    // for each pixel in the image
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            // get the pixel value
            int Y = img.at<Vec3b>(i,j)[0];
            int Cb = img.at<Vec3b>(i,j)[1];
            int Cr = img.at<Vec3b>(i,j)[2];

            //if its the first pixel of the image, do not use prediction
            if (i == 0 && j == 0) {
                YvaluesToBeEncoded.push_back(Y);
                CbvaluesToBeEncoded.push_back(Cb);
                CrvaluesToBeEncoded.push_back(Cr);
                pixel_index++;
            } else if(i==0){
                //if its the first line of the image, use only the previous pixel (to the left)
                int Yerror = Y - img.at<Vec3b>(i,j-1)[0];
                int Cberror = Cb - img.at<Vec3b>(i,j-1)[1];
                int Crerror = Cr - img.at<Vec3b>(i,j-1)[2];
                YvaluesToBeEncoded.push_back(Yerror);
                CbvaluesToBeEncoded.push_back(Cberror);
                CrvaluesToBeEncoded.push_back(Crerror);
                pixel_index++;
            }else if(j==0){
                //if its the first pixel of the line, use only the pixel above
                int Yerror = Y - img.at<Vec3b>(i-1,j)[0];
                int Cberror = Cb - img.at<Vec3b>(i-1,j)[1];
                int Crerror = Cr - img.at<Vec3b>(i-1,j)[2];
                YvaluesToBeEncoded.push_back(Yerror);
                CbvaluesToBeEncoded.push_back(Cberror);
                CrvaluesToBeEncoded.push_back(Crerror);
                pixel_index++;
            } else {
                //if its not the first pixel of the image nor the first line, use the 3 pixels to the left, above and to the left top
                int Yerror = Y - average(img.at<Vec3b>(i,j-1)[0], img.at<Vec3b>(i-1,j)[0], img.at<Vec3b>(i-1,j-1)[0]);
                int Cberror = Cb - average(img.at<Vec3b>(i,j-1)[1], img.at<Vec3b>(i-1,j)[1], img.at<Vec3b>(i-1,j-1)[1]);
                int Crerror = Cr - average(img.at<Vec3b>(i,j-1)[2], img.at<Vec3b>(i-1,j)[2], img.at<Vec3b>(i-1,j-1)[2]);
                YvaluesToBeEncoded.push_back(Yerror);
                CbvaluesToBeEncoded.push_back(Cberror);
                CrvaluesToBeEncoded.push_back(Crerror);
                pixel_index++;
            }

            if(autoMode && pixel_index % bs == 0 && pixel_index != 0) {
                int Ysum = 0;
                int Cbsum = 0;
                int Crsum = 0;
                for(int j = pixel_index - bs; j < pixel_index; j++) {
                    Ysum += abs(YvaluesToBeEncoded[j]);
                    Cbsum += abs(CbvaluesToBeEncoded[j]);
                    Crsum += abs(CrvaluesToBeEncoded[j]);
                }
                int Yu = round(Ysum / bs);
                int Cbu = round(Cbsum / bs);
                int Cru = round(Crsum / bs);
                int Ym = calc_m(Yu);
                int Cbm = calc_m(Cbu);
                int Crm = calc_m(Cru);
                if (Ym < 1) Ym = 1;
                if (Cbm < 1) Cbm = 1;
                if (Crm < 1) Crm = 1;
                Ym_vector.push_back(Ym);
                Cbm_vector.push_back(Cbm);
                Crm_vector.push_back(Crm);
            }
        }
    }

    string YencodedString = "";
    string CbencodedString = "";
    string CrencodedString = "";
    Golomb g;

    //size = rows * cols
    int size = img.rows * img.cols;

    if(!autoMode){
        for(int i = 0; i < size; i++) {
            YencodedString += g.encode(YvaluesToBeEncoded[i], original);
        }
        for(int i = 0; i < size; i++) {
            CbencodedString += g.encode(CbvaluesToBeEncoded[i], original);
        }
        for(int i = 0; i < size; i++) {
            CrencodedString += g.encode(CrvaluesToBeEncoded[i], original);
        }
    } else {
        int m_index = 0;
        for(int i = 0; i < size; i++) {
            if(i % bs == 0 && i != 0) {
                m_index++;
            }
            YencodedString += g.encode(YvaluesToBeEncoded[i], Ym_vector[m_index]);
            CbencodedString += g.encode(CbvaluesToBeEncoded[i], Cbm_vector[m_index]);
            CrencodedString += g.encode(CrvaluesToBeEncoded[i], Crm_vector[m_index]);
        }
    }

    if(!autoMode){
        Ym_vector.clear();
        Cbm_vector.clear();
        Crm_vector.clear();
        Ym_vector.push_back(original);
        Cbm_vector.push_back(original);
        Crm_vector.push_back(original);
    }

    BitStream bitStream(output, "w");

    //write the header
    vector<int> bits;

    //the first 16 bits are the image type
    for(int i = 31; i >= 0; i--) {
        bits.push_back((type >> i) & 1);
    }
    
    //the next 16 bits are the image width
    for(int i = 15; i >= 0; i--) {
        bits.push_back((img.cols >> i) & 1);
    }
    //the next 16 bits are the image height
    for(int i = 15; i >= 0; i--) {
        bits.push_back((img.rows >> i) & 1);
    }
    //the next 16 bits are the block size
    for(int i = 15; i >= 0; i--) {
        bits.push_back((bs >> i) & 1);
    }

    //the next 16 bits are the YencodedString size
    for(int i = 31; i >= 0; i--) {
        bits.push_back((YencodedString.size() >> i) & 1);
    }
    //the next 16 bits are the CbencodedString size
    for(int i = 31; i >= 0; i--) {
        bits.push_back((CbencodedString.size() >> i) & 1);
    }
    
    //the next 32 bits are the CrencodedString size
    for(int i = 31; i >= 0; i--) {
        bits.push_back((CrencodedString.size() >> i) & 1);
    }
    //the next 16 bits are the m_vector size
    for(int i = 15; i >= 0; i--) {
        bits.push_back((Ym_vector.size() >> i) & 1);
    }

    //the next bits are the Ym_vector values (16 bits each)
    //TODO possible representation with only 8 bits (or even 4)
    for(int i = 0; i < Ym_vector.size(); i++) {
        for(int j = 15; j >= 0; j--) {
            bits.push_back((Ym_vector[i] >> j) & 1);
        }
    }
    //the next bits are the Cbm_vector values (16 bits each)
    //TODO possible representation with only 8 bits (or even 4)
    for(int i = 0; i < Cbm_vector.size(); i++) {
        for(int j = 15; j >= 0; j--) {
            bits.push_back((Cbm_vector[i] >> j) & 1);
        }
    }
    //the next bits are the Crm_vector values (16 bits each)
    //TODO possible representation with only 8 bits (or even 4)
    for(int i = 0; i < Crm_vector.size(); i++) {
        for(int j = 15; j >= 0; j--) {
            bits.push_back((Crm_vector[i] >> j) & 1);
        }
    }

    //contains all the bits from the values to be encoded vectors (Y, Cb, Cr)
    vector<int> encoded_bits;

    for(int i = 0; i < YencodedString.length(); i++) {
        encoded_bits.push_back(YencodedString[i] - '0');
    }
    for(int i = 0; i < CbencodedString.length(); i++) {
        encoded_bits.push_back(CbencodedString[i] - '0');
    }
    for(int i = 0; i < CrencodedString.length(); i++) {
        encoded_bits.push_back(CrencodedString[i] - '0');
    }

    //the next bits are the encoded bits
    for(int i = 0; i < encoded_bits.size(); i++) {
        bits.push_back(encoded_bits[i]);
    }

    //write the bits to the file
    bitStream.writeBits(bits);
    bitStream.close();
    

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;
 
    return 0;
}

