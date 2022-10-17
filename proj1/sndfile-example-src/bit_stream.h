#include <fstream>

// C++ can only write bytes not bits. So we need to write a byte at a time.
class BitStream {
    private:
        std::fstream file;
        string fileName;
        string bitstream = "";

    public:
        BitStream(string name) {
            fileName = name;
        }

        string readBits(int n) {
            file.open(fileName, std::ios::in);
            
            // Check if file has enough characters
            if(file.peek() == EOF) {
                cout << "Error: file is empty\n";
                return;
            }

            // Check if the file has n characters
            for(int i = 0; i < n; i++) {
                if(file.peek() == EOF) {
                    cout << "Error: file does not have enough characters\n";
                    return;
                }
                file.get();
            }

            // Append characters to bitstream
            for (int i = 0; i < n; i++) {
                bitstream += file.get();
            }

            file.close();

            return bitstream;
        }

        void readBit() {
            return readBits(1);
        }

        // void writeBits(int n) {
        //     std::bitset<sizeof(unsigned long) * 8> bits(zeroes_and_ones);

        //     unsigned long binary_value = bits.to_ulong();

        //     // Write the binary value to file
        //     codedFile.write((const char*)&binary_value, sizeof(unsigned long));
        // }

        void writeBit(bool bit) {
        }
}
