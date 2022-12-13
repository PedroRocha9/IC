#include "golomb.h"
#include "bitStream.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include "YUV4MPEG2Reader.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

    //function that converts char to array of bits
    auto charToBits = [](char c) {
        vector<int> bits;
        for (int i = 7; i >= 0; i--) {
            bits.push_back((c >> i) & 1);
        }
        return bits;
    };

    //function to calculate m based on u
    auto calc_m = [](int u) {
        //u = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) u / (1 + u)));
    };

    auto predict = [](int a, int b, int c, int mode) {
        if (c >= max(a, b))
            return min(a, b);   //min(left, above) if  left top >= max(left, above)
        else if (c <= min(a, b))
            return max(a, b);   //max(left, above) if  left top <= min(left, above)
        else
            return a + b - c;   //otherwise, left + above - left top
    };

    //This program will encode a video file in the format of YUV4MPEG2 into a binary file using intraframe encoding

    //Check for correct number of arguments
    if(argc != 4){
        cout << "Usage: " << argv[0] << " <input file> <output file> <block size>" << endl;
        return 1;
    }

    //Open input file
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        cout << "Error: Could not open input file" << endl;
        return 1;
    }

    int blockSize = atoi(argv[3]);
    

    YUV4MPEG2Reader reader(argv[1]);

    int width = reader.width();
    int height = reader.height();
    int colorSpace = stoi(reader.colorSpace());
    int aspectRatio1 = reader.aspectRatio1();
    int aspectRatio2 = reader.aspectRatio2();
    char interlace = reader.interlacing().at(0);
    int frameRate1 = reader.frameRate1();
    int frameRate2 = reader.frameRate2();

    // cout << "Width: " << width << endl;
    // cout << "Height: " << height << endl;
    // cout << "Color Space: " << colorSpace << endl;
    // cout << "Aspect Ratio: " << aspectRatio1 << endl;
    // cout << "Aspect Ratio: " << aspectRatio2 << endl;
    // cout << "interlace: " << interlace << endl;
    // cout << "Frame Rate: " << frameRate1 << endl;
    // cout << "Frame Rate: " << frameRate2 << endl;

    vector<int> Y, U, V;

    string Yencoded = "";
    string Cbencoded = "";
    string Crencoded = "";

    vector<short> Ym, Cbm, Crm;

    vector<int> encoded_Ybits;
    vector<int> encoded_Cbbits;
    vector<int> encoded_Crbits;

    //start a timer
    clock_t start = clock();

    int numFrames = 0;

    if(colorSpace == 420){
        //The file is in 4:2:0 format
        //The file contains a line of "FRAME" followed by a new line and then the YUV data
        //The YUV data is in the order of Y, U, V

        vector<int> Ym_vector;
        vector<int> Cbm_vector;
        vector<int> Crm_vector;
        vector<int> Yresiduals;
        vector<int> Cbresiduals;
        vector<int> Crresiduals;
        
        bool finish = false;

        Y = vector<int>(width * height);
        U = vector<int>(width * height / 4);
        V = vector<int>(width * height / 4);
        //Read the line after the header
        char line[100];
        fgets(line, 100, input);

        while(!feof(input)){
            numFrames++;
            //read the frame line
            fgets(line, 100, input);
            //read the Y data
            for(int i = 0; i < width * height; i++){
                Y[i] = fgetc(input);   
                if(Y[i] < 0) {
                    numFrames--;
                    finish = true;
                    break;
                }
            }
            if (finish) break;
            //read the U data
            for(int i = 0; i < width * height / 4; i++){
                U[i] = fgetc(input);
            }
            //read the V data
            for(int i = 0; i < width * height / 4; i++){
                V[i] = fgetc(input);
            }

            //Create Mat objects for Y, U, and V
            Mat YMat = Mat(height, width, CV_8UC1);
            Mat UMat = Mat(height/2, width/2, CV_8UC1);
            Mat VMat = Mat(height/2, width/2, CV_8UC1);

            //copy the Y, U, and V data into the Mat objects
            for(int i = 0; i < height; i++){
                for(int j = 0; j < width; j++){
                    YMat.at<uchar>(i, j) = Y[i * width + j];
                }
                if (i < height/2 && i < width/2) {
                    for(int j = 0; j < width/2; j++){
                        UMat.at<uchar>(i, j) = U[i * width/2 + j];
                        VMat.at<uchar>(i, j) = V[i * width/2 + j];
                    }
                }
            }

            //convert the Y, U, and V Mat objects back to vectors by reversing the process
            vector<int> Y_vector;
            vector<int> Cb_vector;
            vector<int> Cr_vector;
            for(int i = 0; i < height; i++){
                for(int j = 0; j < width; j++){
                    Y_vector.push_back(YMat.at<uchar>(i, j));
                }
                if (i < height/2 && i < width/2) {
                    for(int j = 0; j < width/2; j++){
                        Cb_vector.push_back(UMat.at<uchar>(i, j));
                        Cr_vector.push_back(VMat.at<uchar>(i, j));
                    }
                }
            }

            // if(numFrames < 301){
            //     for(int i = 0; i < height; i++){
            //         for(int j = 0; j < width; j++){
            //             cout << (int)YMat.at<uchar>(i, j) << endl;
            //         }
            //     }
            // }

            //go pixel by pixel through the Y, U, and V Mat objects to make predictions
            for(int i = 0; i < height; i++){
                for(int j = 0; j < width; j++){
                    int Y = YMat.at<uchar>(i, j);
                    int U = UMat.at<uchar>(i, j);
                    int V = VMat.at<uchar>(i, j);
                    //if its the first pixel of the image, do not use prediction
                    if (i == 0 && j == 0) {
                        Yresiduals.push_back(Y);
                        Cbresiduals.push_back(U);
                        Crresiduals.push_back(V);
                    } else if(i==0){
                        //if its the first line of the image, use only the previous pixel (to the left)
                        Yresiduals.push_back(Y - YMat.at<uchar>(i, j-1));
                        if (j < (width/2)) {
                            Cbresiduals.push_back(U - UMat.at<uchar>(i, j-1));
                            Crresiduals.push_back(V - VMat.at<uchar>(i, j-1));
                        }
                    } else if(j==0){
                        //if its the first pixel of the line, use only the pixel above
                        Yresiduals.push_back(Y - YMat.at<uchar>(i-1, j));
                        if (i < (height/2)) {
                            Cbresiduals.push_back(U - UMat.at<uchar>(i-1, j));
                            Crresiduals.push_back(V - VMat.at<uchar>(i-1, j));
                        }
                    } else {
                        //otherwise, use the prediction function
                        Yresiduals.push_back(Y - predict(YMat.at<uchar>(i, j-1), YMat.at<uchar>(i-1, j), YMat.at<uchar>(i-1, j-1), 0));
                        if (i < (height/2) && j < (width/2)) {
                            Cbresiduals.push_back(U - predict(UMat.at<uchar>(i, j-1), UMat.at<uchar>(i-1, j), UMat.at<uchar>(i-1, j-1), 0));
                            Crresiduals.push_back(V - predict(VMat.at<uchar>(i, j-1), VMat.at<uchar>(i-1, j), VMat.at<uchar>(i-1, j-1), 0));
                        }
                    }   
                }
            }
            //PADDING TO VECTORS
            if (Yresiduals.size() % blockSize != 0) {
                int padding = blockSize - (Yresiduals.size() % blockSize);
                for (int i = 0; i < padding; i++) {
                    Yresiduals.push_back(0);
                }
            }
            if (Cbresiduals.size() % blockSize != 0) {
                int padding = blockSize - (Cbresiduals.size() % blockSize);
                for (int i = 0; i < padding; i++) {
                    Cbresiduals.push_back(0);
                    Crresiduals.push_back(0);
                }
            }
            //M VECTOR CALCULATION
            for(int i = 0; i < Yresiduals.size(); i++){
                if(i % blockSize == 0 and i != 0){
                    int sum = 0;
                    for(int j = i - blockSize; j < i; j++){
                        sum += abs(Yresiduals[j]);
                    }
                    int mean = sum / blockSize;
                    int m = calc_m(mean);
                    if (m == 0) m = 1;
                    Ym_vector.push_back(m);
                }
                if(i == Yresiduals.size() - 1){
                    int sum = 0;
                    for(int j = i - (i % blockSize); j < i; j++){
                        sum += abs(Yresiduals[j]);
                    }
                    int mean = sum / (i % blockSize);
                    int m = calc_m(mean);
                    if (m == 0) m = 1;
                    Ym_vector.push_back(m);
                }
                if(i % blockSize == 0 and i != 0 and i < Cbresiduals.size()){
                    int sumCb = 0;
                    int sumCr = 0;
                    for(int j = i - blockSize; j < i; j++){
                        sumCb += abs(Cbresiduals[j]);
                        sumCr += abs(Crresiduals[j]);
                    }
                    int meanCb = sumCb / blockSize;
                    int meanCr = sumCr / blockSize;
                    int mCb = calc_m(meanCb);
                    int mCr = calc_m(meanCr);
                    if (mCb == 0) mCb = 1;
                    if (mCr == 0) mCr = 1;
                    Cbm_vector.push_back(mCb);
                    Crm_vector.push_back(mCr);
                }
                if(i == Cbresiduals.size() - 1){
                    int sumCb = 0;
                    int sumCr = 0;
                    for(int j = i - (i % blockSize); j < i; j++){
                        sumCb += abs(Cbresiduals[j]);
                        sumCr += abs(Crresiduals[j]);
                    }
                    int meanCb = sumCb / (i % blockSize);
                    int meanCr = sumCr / (i % blockSize);
                    int mCb = calc_m(meanCb);
                    int mCr = calc_m(meanCr);
                    if (mCb == 0) mCb = 1;
                    if (mCr == 0) mCr = 1;
                    Cbm_vector.push_back(mCb);
                    Crm_vector.push_back(mCr);
                }
            }
            // cout << "Yresiduals.size() = " << Yresiduals.size() << endl;
            // cout << "Cbresiduals.size() = " << Cbresiduals.size() << endl;
            // cout << "Crresiduals.size() = " << Crresiduals.size() << endl;
            // cout << "Ym_vector.size() = " << Ym_vector.size() << endl;
            // cout << "Cbm_vector.size() = " << Cbm_vector.size() << endl;
            // cout << "Crm_vector.size() = " << Crm_vector.size() << endl;
            Golomb g;
            int m_index = 0;
            for(int i = 0; i < Yresiduals.size(); i++){
                
                if(i % blockSize == 0 and i != 0){
                    Ym.push_back(Ym_vector[m_index]);
                    m_index++;
                }
                Yencoded += g.encode(Yresiduals[i], Ym_vector[m_index]);
                if (i == Yresiduals.size() - 1) Ym.push_back(Ym_vector[m_index]);
            }
            m_index = 0;
            for(int i = 0; i < Cbresiduals.size(); i++){
                if(i % blockSize == 0 and i != 0) {
                    Cbm.push_back(Cbm_vector[m_index]);
                    Crm.push_back(Crm_vector[m_index]);
                    m_index++;
                }
                Cbencoded += g.encode(Cbresiduals[i], Cbm_vector[m_index]);
                Crencoded += g.encode(Crresiduals[i], Crm_vector[m_index]);
                if (i == Cbresiduals.size() - 1) {
                    Cbm.push_back(Cbm_vector[m_index]);
                    Crm.push_back(Crm_vector[m_index]);
                }
            }
            Ym_vector = vector<int>();
            Cbm_vector = vector<int>();
            Crm_vector = vector<int>();
            Yresiduals = vector<int>();
            Cbresiduals = vector<int>();
            Crresiduals = vector<int>();

            for (long unsigned int i = 0; i < Yencoded.length(); i++)
                encoded_Ybits.push_back(Yencoded[i] - '0');
            for (long unsigned int i = 0; i < Cbencoded.length(); i++)
                encoded_Cbbits.push_back(Cbencoded[i] - '0');
            for (long unsigned int i = 0; i < Crencoded.length(); i++)
                encoded_Crbits.push_back(Crencoded[i] - '0');

            Yencoded = "";
            Cbencoded = "";
            Crencoded = "";
            // if (numFrames == 4) break;

        } //end of loop for each frame
    }
    // cout << "Finished encoding" << endl;
    // cout << "numFrames = " << numFrames << endl;
    // cout << "encoded_Ybits.size() = " << encoded_Ybits.size() << endl;
    // cout << "encoded_Cbbits.size() = " << encoded_Cbbits.size() << endl;
    // cout << "encoded_Crbits.size() = " << encoded_Crbits.size() << endl;
    // cout << "Ym.size() = " << Ym.size() << endl;
    // cout << "Cbm.size() = " << Cbm.size() << endl;
    // cout << "Crm.size() = " << Crm.size() << endl;

    BitStream bs(argv[2], "w");
    vector<int> bits;
    //the first 16 bits are the width of the image
    for (int i = 15; i >= 0; i--) {
        bits.push_back((width >> i) & 1);
    }
    //the next 16 bits are the height of the image
    for (int i = 15; i >= 0; i--) {
        bits.push_back((height >> i) & 1);
    }
    //the next 16 bits are the number of frames
    for (int i = 15; i >= 0; i--) {
        bits.push_back((numFrames >> i) & 1);
    }
    //the next 16 bits are the color space
    for (int i = 15; i >= 0; i--) {
        bits.push_back((colorSpace >> i) & 1);
    }
    // the next 16 bits are aspect ratio 1
    for (int i = 15; i >= 0; i--) {
        bits.push_back((aspectRatio1 >> i) & 1);
    }
    // the next 16 bits are aspect ratio 2
    for (int i = 15; i >= 0; i--) {
        bits.push_back((aspectRatio2 >> i) & 1);
    }
    //the next 16 bits are the frame rate 1
    for (int i = 15; i >= 0; i--) {
        bits.push_back((frameRate1 >> i) & 1);
    }
    //the next 16 bits are the frame rate 2
    for (int i = 15; i >= 0; i--) {
        bits.push_back((frameRate2 >> i) & 1);
    }
    vector<int> interlace_v = charToBits(interlace);
    for(int i = 0; i < 8; i++){
        bits.push_back(interlace_v[i]);
    }
    //the next 8 bits are the block size
    for (int i = 7; i >= 0; i--) {
        bits.push_back((blockSize >> i) & 1);
    }
    //the next 32 bits are the encoded_Ybits.size()
    for (int i = 31; i >= 0; i--) {
        bits.push_back((encoded_Ybits.size() >> i) & 1);
    }
    //the next 32 bits are the encoded_Cbbits.size()
    for (int i = 31; i >= 0; i--) {
        bits.push_back((encoded_Cbbits.size() >> i) & 1);
    }
    //the next 32 bits are the encoded_Crbits.size()
    for (int i = 31; i >= 0; i--) {
        bits.push_back((encoded_Crbits.size() >> i) & 1);
    }
    //the next 32 bits are the Ym.size()
    for (int i = 31; i >= 0; i--) {
        bits.push_back((Ym.size() >> i) & 1);
    }
    //the next 32 bits are the Cbm.size()
    for (int i = 31; i >= 0; i--) {
        bits.push_back((Cbm.size() >> i) & 1);
    }
    //the next 32 bits are the Crm.size()
    for (int i = 31; i >= 0; i--) {
        bits.push_back((Crm.size() >> i) & 1);
    }
    //the next bits are the Ym values
    for (long unsigned int i = 0; i < Ym.size(); i++) {
        //the next 8 bits are the Ym value
        for (int j = 7; j >= 0; j--) {
            bits.push_back((Ym[i] >> j) & 1);
        }
    }
    //the next bits are the Cbm values
    for (long unsigned int i = 0; i < Cbm.size(); i++) {
        //the next 8 bits are the Cbm value
        for (int j = 7; j >= 0; j--) {
            bits.push_back((Cbm[i] >> j) & 1);
        }
    }
    //the next bits are the Crm values
    for (long unsigned int i = 0; i < Crm.size(); i++) {
        //the next 8 bits are the Crm value
        for (int j = 7; j >= 0; j--) {
            bits.push_back((Crm[i] >> j) & 1);
        }
    }
    //the next bits are the encoded_Ybits
    for(long unsigned int i = 0; i < encoded_Ybits.size(); i++) {
        bits.push_back(encoded_Ybits[i]);
    }
    //the next bits are the encoded_Cbbits
    for(long unsigned int i = 0; i < encoded_Cbbits.size(); i++) {
        bits.push_back(encoded_Cbbits[i]);
    }
    //the next bits are the encoded_Crbits
    for(long unsigned int i = 0; i < encoded_Crbits.size(); i++) {
        bits.push_back(encoded_Crbits[i]);
    }

    bs.writeBits(bits);
    bs.close();


    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;
    return 0;
}