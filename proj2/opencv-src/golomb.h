// Golomb class
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <cmath>

// C++ can only write bytes not bits. So we need to write a byte at a time.
class Golomb {
    private:
        int m = 5;
        int min_bits;
        int max_bits;
        int n_values_with_min_bits;
        std::vector<std::string> remainders_table;

        // calculate max_bits: log2(m) with ceiling to integer, min_bits and values with min_bits
        void calculateBits() {
            max_bits = ceil(log2(m));
            min_bits = max_bits - 1;
            n_values_with_min_bits = pow(2, max_bits) - m;
        }

        // return array of strings based on golomb
        std::vector<std::string> calculateRemaindersTable(){
            std::vector<std::string> remainders;
            for (int i = 0; i < m; i++) {
                std::string tmp;
                if (i < n_values_with_min_bits) {
                    tmp = intToBitString(i, min_bits);
                } 
                else {
                    tmp = intToBitString(i + n_values_with_min_bits, max_bits);
                }
                remainders.push_back(tmp);
            }
            return remainders;


        }
        

    public:
        // constructor with m as parameter
        Golomb(int m) {
            this->m = m;
            calculateBits();
            remainders_table = calculateRemaindersTable();
            // print remainders_table
            for (int i = 0; i < remainders_table.size(); i++) {
                std::cout << remainders_table[i] << std::endl;
            }
        }

        // function that converts string of bits to integer
        int bitStringToInt(std::string bitString) {
            int result = 0;
            for (int i = 0; i < bitString.length(); i++) {
                result = result * 2 + (bitString[i] - '0');
            }
            return result;
        }
        
        // function that converts integer to string of bits with n bits of representation
        std::string intToBitString(int num, int n_bits_representation){
            std::string result = "";
            for (int i = 0; i < n_bits_representation; i++) {
                result = std::to_string(num % 2) + result;
                num /= 2;
            }
            return result;
        }
        
        // with m bits, we know the number of remainders we can have
        
};
