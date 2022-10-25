#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <bitset>

// C++ can only write bytes not bits. So we need to write a byte at a time.
class BitStream {
    private:
        std::fstream file;
        std::string fileMode;
        std::string fileName;
        int fileSize;
        //create a variable that hold multiple arrays of 8 bits
        std::vector<std::vector<int>> byteArray;
        //create a variable that holds the current array of 8 bits
        std::vector<int> bitArray;
        //create a variable that holds the current bit in the current array
        int currentBitPos;
        //create a variable that holds the current array in the byteArray
        int currentArrayPos;
        


    public:
        BitStream(std::string name, std::string mode) {
            fileName = name;
            fileMode = mode;

            if (mode == "r"){
                file.open(fileName, std::ios::in | std::ios::binary);
                currentBitPos = 0;
                fileSize = getFileSize();

            } else if (mode == "w") {
                file.open(fileName, std::ios::out | std::ios::binary);
                currentBitPos = 0;
                currentArrayPos = 0;
            } else {
                std::cout << "Invalid mode" << std::endl;
            }

        }

        int getFileSize() {
            // file.seekg(0, std::ios::end);
            // int size = file.tellg();
            // file.seekg(0, std::ios::beg);

            //create a duplicate file stream
            std::fstream file2;
            file2.open(fileName, std::ios::in | std::ios::binary);
            file2.seekg(0, std::ios::end);
            int size = file2.tellg();
            file2.seekg(0, std::ios::beg);
            file2.close();
            return size;
        }

        std::vector<int> byteToBitArray(char byte){
            if (fileMode == "r"){
                //read the byte and convert it to a bit array 
                std::vector<int> bitArray;
                for (int i = 0; i < 8; i++){
                    bitArray.push_back((byte >> i) & 1);
                }

                //reverse the bit array
                std::vector<int> reversedBitArray;
                for (int i = 7; i >= 0; i--){
                    reversedBitArray.push_back(bitArray[i]);
                }

                return reversedBitArray;
            }

            std::vector<int> bitArray;
            for (int i = 0; i < 8; i++) {
                bitArray.push_back(byte & 1);
                byte >>= 1;
            }
            return bitArray;
        }

        char bitArrayToByte(std::vector<int> bitArray){
            char byte = 0;
            //invert the bit array
            std::vector<int> invertedBitArray;
            for (int i = 7; i >= 0; i--){
                invertedBitArray.push_back(bitArray[i]);
            }

            for (int i = 0; i < 8; i++){
                byte |= invertedBitArray[i] << i;
            }

            return byte;
        }

        std::vector<int> readBits(int n) {
            if (fileMode != "r") {
                std::cout << "File not open for reading" << std::endl;
                return std::vector<int>();
            }
            
            std::vector <int> outBits;

            char byte;
            int bitCount = 0;
            while (bitCount < n) {
                if (currentBitPos == 0) {
                    file.read(&byte, 1);
                    bitArray = byteToBitArray(byte);
                }
                outBits.push_back(bitArray[currentBitPos]);
                currentBitPos++;
                bitCount++;
                if (currentBitPos == 8) {
                    currentBitPos = 0;
                }
            }
            return outBits;

            
        }

        int readBit() {
            if (fileMode != "r") {
                std::cout << "File not open for reading" << std::endl;
                return -1;
            }
            if (currentBitPos == 0) {
                char byte;
                file.read(&byte, 1);
                bitArray = byteToBitArray(byte);
            }
            int bit = bitArray[currentBitPos];
            currentBitPos = (currentBitPos + 1) % 8;
            return bit;
            

        }

        void writeBits(std::vector<int> bits) {

            if (fileMode != "w"){
                std::cout << "File is not open for writing" << std::endl;
                return;
            }
            //this function writes the next n bits to the file
            //open the file and write the next n bits
            //n is size of bits
            int n = bits.size();

            // std::cout << "n: " << n-64 << std::endl;

            int bitCount = 0;
            while (n > 0){
                if (currentBitPos == 8){
                //reset the current bit position
                //go to the next array in byteArray
                currentArrayPos++;
                currentBitPos = 0;
                }
                //if the current bit position is 0, then we need to create a new array
                if (currentBitPos == 0){
                    bitArray = std::vector<int>(8);
                    byteArray.push_back(bitArray);
                }
                //write the next bit to the current array
                bitArray[currentBitPos] = bits[bitCount];
                //add bitArray to the currentArrayPos in byteArray
                byteArray[currentArrayPos] = bitArray;
                currentBitPos++;
                bitCount++;
                n--;

            }
        }

        void writeBit(int bit) {

            if (fileMode != "w"){
                std::cout << "File is not open for writing" << std::endl;
                return;
            }
            //this function writes the next bit to the file
            //open the file and write the next bit
            //if the current bit position is 8, then we need to change it to 0
            if (currentBitPos == 8){
                //reset the current bit position
                //go to the next array in byteArray
                currentArrayPos++;
                currentBitPos = 0;
            }
            //if the current bit position is 0, then we need to create a new array
            if (currentBitPos == 0){
                bitArray = std::vector<int>(8);
                byteArray.push_back(bitArray);
            }
            //write the next bit to the current array
            bitArray[currentBitPos] = bit;
            //add bitArray to the currentArrayPos in byteArray
            byteArray[currentArrayPos] = bitArray;
            currentBitPos++;
            
        }


        void close(){

            if (fileMode == "w"){
                //write the byteArray to the file
                for (int i = 0; i < byteArray.size(); i++){
                    char byte = 0;
                    //inversed
                    for (int j = 0; j < 8; j++){
                        byte = byte << 1;
                        byte = byte | byteArray[i][j];
                    }
                    file.write(&byte, 1);
                }
            }
            file.close();
        }
};
