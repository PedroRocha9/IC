#include "bitStream.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include "YUV4MPEG2Reader.h"
#include "golomb.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){
    //function that converts array of bits to char
    auto bits_to_char = [](vector<int> bits) {
        int c = 0;
        for(long unsigned int i = 0; i < bits.size(); i++) c += bits[i] * pow(2, bits.size() - i - 1);
        return c;
    };

    //function to calculate m based on u
    auto calc_m = [](int u) {
        //u = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) u / (1 + u)));
    };

    auto predict = [](int a, int b, int c) {
        if (c >= max(a, b))
            return min(a, b);   //min(left, above) if  left top >= max(left, above)
        else if (c <= min(a, b))
            return max(a, b);   //max(left, above) if  left top <= min(left, above)
        else
            return a + b - c;   //otherwise, left + above - left top
    };

    //start a timer
    clock_t start = clock();

    if(argc != 3){
        cout << "Usage: " << argv[0] << " <input file> <output file>" << endl;
        return 1;
    }
    string input_file = argv[1];
    BitStream bs(input_file, "r");
    string output_file = argv[2];

    //The output file is a YUV4MPEG2 file
    //write the header
    ofstream out(output_file, ios::out | ios::binary);

    vector<int> v_width = bs.readBits(16);
    vector<int> v_height = bs.readBits(16);
    vector<int> v_num_frames = bs.readBits(16);
    vector<int> v_color_space = bs.readBits(16);
    vector<int> v_aspect_ratio_1 = bs.readBits(16);
    vector<int> v_aspect_ratio_2 = bs.readBits(16);
    vector<int> v_frame_rate_1 = bs.readBits(16);
    vector<int> v_frame_rate_2 = bs.readBits(16);
    vector<int> v_interlace = bs.readBits(8);
    vector<int> v_bs = bs.readBits(8);
    vector<int> v_Ybits_size = bs.readBits(32);
    vector<int> v_Cbbits_size = bs.readBits(32);
    vector<int> v_Crbits_size = bs.readBits(32);
    vector<int> v_Ym_size = bs.readBits(32);
    vector<int> v_Cbm_size = bs.readBits(32);
    vector<int> v_Crm_size = bs.readBits(32);

    int width = 0;
    for(long unsigned int i = 0; i < v_width.size(); i++)
        width += v_width[i] * pow(2, v_width.size() - i - 1);

    int height = 0;
    for(long unsigned int i = 0; i < v_height.size(); i++)
        height += v_height[i] * pow(2, v_height.size() - i - 1);

    int num_frames = 0;
    for(long unsigned int i = 0; i < v_num_frames.size(); i++)
        num_frames += v_num_frames[i] * pow(2, v_num_frames.size() - i - 1);

    int color_space = 0;
    for(long unsigned int i = 0; i < v_color_space.size(); i++)
        color_space += v_color_space[i] * pow(2, v_color_space.size() - i - 1);

    int aspect_ratio_1 = 0;
    for(long unsigned int i = 0; i < v_aspect_ratio_1.size(); i++)
        aspect_ratio_1 += v_aspect_ratio_1[i] * pow(2, v_aspect_ratio_1.size() - i - 1);

    int aspect_ratio_2 = 0;
    for(long unsigned int i = 0; i < v_aspect_ratio_2.size(); i++)
        aspect_ratio_2 += v_aspect_ratio_2[i] * pow(2, v_aspect_ratio_2.size() - i - 1);

    int frame_rate_1 = 0;
    for(long unsigned int i = 0; i < v_frame_rate_1.size(); i++)
        frame_rate_1 += v_frame_rate_1[i] * pow(2, v_frame_rate_1.size() - i - 1);

    int frame_rate_2 = 0;
    for(long unsigned int i = 0; i < v_frame_rate_2.size(); i++)
        frame_rate_2 += v_frame_rate_2[i] * pow(2, v_frame_rate_2.size() - i - 1);

    char interlace = bits_to_char(v_interlace);

    int bs_size = 0;
    for(long unsigned int i = 0; i < v_bs.size(); i++)
        bs_size += v_bs[i] * pow(2, v_bs.size() - i - 1);

    int Ybits_size = 0;
    for(long unsigned int i = 0; i < v_Ybits_size.size(); i++)
        Ybits_size += v_Ybits_size[i] * pow(2, v_Ybits_size.size() - i - 1);

    int Cbbits_size = 0;
    for(long unsigned int i = 0; i < v_Cbbits_size.size(); i++)
        Cbbits_size += v_Cbbits_size[i] * pow(2, v_Cbbits_size.size() - i - 1);

    int Crbits_size = 0;
    for(long unsigned int i = 0; i < v_Crbits_size.size(); i++)
        Crbits_size += v_Crbits_size[i] * pow(2, v_Crbits_size.size() - i - 1);

    int Ym_size = 0;
    for(long unsigned int i = 0; i < v_Ym_size.size(); i++)
        Ym_size += v_Ym_size[i] * pow(2, v_Ym_size.size() - i - 1);

    int Cbm_size = 0;
    for(long unsigned int i = 0; i < v_Cbm_size.size(); i++)
        Cbm_size += v_Cbm_size[i] * pow(2, v_Cbm_size.size() - i - 1);

    int Crm_size = 0;
    for(long unsigned int i = 0; i < v_Crm_size.size(); i++)
        Crm_size += v_Crm_size[i] * pow(2, v_Crm_size.size() - i - 1);

    // cout << "width: " << width << endl;
    // cout << "height: " << height << endl;
    // cout << "num_frames: " << num_frames << endl;
    // cout << "color_space: " << color_space << endl;
    // cout << "aspect_ratio_1: " << aspect_ratio_1 << endl;
    // cout << "aspect_ratio_2: " << aspect_ratio_2 << endl;
    // cout << "frame_rate_1: " << frame_rate_1 << endl;
    // cout << "frame_rate_2: " << frame_rate_2 << endl;
    // cout << "interlace: " << interlace << endl;
    // cout << "bs_size: " << bs_size << endl;
    // cout << "Ybits_size: " << Ybits_size << endl;
    // cout << "Cbbits_size: " << Cbbits_size << endl;
    // cout << "Crbits_size: " << Crbits_size << endl;
    // cout << "Ym_size: " << Ym_size << endl;
    // cout << "Cbm_size: " << Cbm_size << endl;
    // cout << "Crm_size: " << Crm_size << endl;

    if(color_space != 420)
        out << "YUV4MPEG2 W" << width << " H" << height << " F" << frame_rate_1 << ":" << frame_rate_2 << " Ip A" << aspect_ratio_1 << ":" << aspect_ratio_2 << " C" << color_space << endl;
    else 
        out << "YUV4MPEG2 W" << width << " H" << height << " F" << frame_rate_1 << ":" << frame_rate_2 << " Ip A" << aspect_ratio_1 << ":" << aspect_ratio_2 << endl;

    //write to the file FRAME
    out << "FRAME" << endl;

    vector<int> Ym;
    for(int i = 0; i < Ym_size; i++){
        vector<int> v_Ym = bs.readBits(8);
        int Ym_i = 0;
        for (long unsigned int j = 0; j < v_Ym.size(); j++){
            Ym_i += v_Ym[j] * pow(2, v_Ym.size() - j - 1);
        }
        Ym.push_back(Ym_i);
    }

    vector<int> Cbm;
    for(int i = 0; i < Cbm_size; i++){
        vector<int> v_Cbm = bs.readBits(8);
        int Cbm_i = 0;
        for (long unsigned int j = 0; j < v_Cbm.size(); j++){
            Cbm_i += v_Cbm[j] * pow(2, v_Cbm.size() - j - 1);
        }
        Cbm.push_back(Cbm_i);
    }

    vector<int> Crm;
    for(int i = 0; i < Crm_size; i++){
        vector<int> v_Crm = bs.readBits(8);
        int Crm_i = 0;
        for (long unsigned int j = 0; j < v_Crm.size(); j++){
            Crm_i += v_Crm[j] * pow(2, v_Crm.size() - j - 1);
        }
        Crm.push_back(Crm_i);
    }

    vector<int> Ybits = bs.readBits(Ybits_size);
    vector<int> Cbbits = bs.readBits(Cbbits_size);
    vector<int> Crbits = bs.readBits(Crbits_size);

    string Yencodedstring = "";
    for(long unsigned int i = 0; i < Ybits.size(); i++){
        Yencodedstring += to_string(Ybits[i]);
    }
    string Cbencodedstring = "";
    for(long unsigned int i = 0; i < Cbbits.size(); i++){
        Cbencodedstring += to_string(Cbbits[i]);
    }
    string Crencodedstring = "";
    for(long unsigned int i = 0; i < Crbits.size(); i++){
        Crencodedstring += to_string(Crbits[i]);
    }

    Golomb g;
    vector<int> Ydecoded = g.decodeMultiple(Yencodedstring, Ym, bs_size);
    vector<int> Cbdecoded = g.decodeMultiple(Cbencodedstring, Cbm, bs_size);
    vector<int> Crdecoded = g.decodeMultiple(Crencodedstring, Crm, bs_size);

    // for(int i = 0; i < 101376; i++){
    //     cout << Ydecoded[i] << endl;
    // }

    Mat YMat = Mat(height, width, CV_8UC1);
    Mat UMat = Mat(height/2, width/2, CV_8UC1);
    Mat VMat = Mat(height/2, width/2, CV_8UC1);

    //undo the predictions
    int pixel_idx = 0;
    int pixel_idx2 = 0;
    for(int n = 0; n < num_frames; n++){
        YMat = Mat(height, width, CV_8UC1);
        UMat = Mat(height/2, width/2, CV_8UC1);
        VMat = Mat(height/2, width/2, CV_8UC1);
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(i == 0 && j == 0){
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx];
                    UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2];
                    VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2];
                    pixel_idx++;
                    pixel_idx2++;
                } else if(i == 0){
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i, j-1);
                    if(j < (width/2)){
                        UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + UMat.at<uchar>(i, j-1);
                        VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2] + VMat.at<uchar>(i, j-1);
                        pixel_idx2++;
                    }
                    pixel_idx++;
                } else if(j == 0){
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i-1, j);
                    if(i < (height/2)){
                        UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + UMat.at<uchar>(i-1, j);
                        VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2] + VMat.at<uchar>(i-1, j);
                        pixel_idx2++;
                    }
                    pixel_idx++;
                } else {
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + predict(YMat.at<uchar>(i, j-1), YMat.at<uchar>(i-1, j), YMat.at<uchar>(i-1, j-1));
                    if(i < (height/2) && j < (width/2)){
                        UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + predict(UMat.at<uchar>(i, j-1), UMat.at<uchar>(i-1, j), UMat.at<uchar>(i-1, j-1));
                        VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2] + predict(VMat.at<uchar>(i, j-1), VMat.at<uchar>(i-1, j), VMat.at<uchar>(i-1, j-1));
                        pixel_idx2++;
                    }
                    pixel_idx++;
                }
            }
        }
        // for(int i = 0; i < height; i++){
        //     for(int j = 0; j < width; j++){
        //         cout << (int)YMat.at<uchar>(i, j) << endl;
        //     }
        // }

        //convert the matrix back to a vector
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

        //write the Y_vector to the file
        for(int i = 0; i < Y_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Y_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }

        //write the Cb_vector to the file
        for(int i = 0; i < Cb_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Cb_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }

        //write the Cr_vector to the file
        for(int i = 0; i < Cr_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Cr_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }
        if(n < num_frames - 1)
            out << "FRAME" << endl;
    }

    //close the file
    out.close();
    

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    //convert the time to milliseconds
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;
    return 0;
}