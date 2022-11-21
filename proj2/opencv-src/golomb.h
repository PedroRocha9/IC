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
        // int m = 5;
        int min_bits;
        int max_bits;
        int n_values_with_min_bits;
        std::vector<std::string> remainders_table;

        // calculate max_bits: log2(m) with ceiling to integer, min_bits and values with min_bits
        void calculateBits(int m) {
            max_bits = ceil(log2(m));
            min_bits = max_bits - 1;
            n_values_with_min_bits = pow(2, max_bits) - m;
        }

        // return array of strings based on golomb
        std::vector<std::string> calculateRemaindersTable(int m){
            std::vector<std::string> remainders;
            for (int i = 0; i < m; i++) {
                std::string tmp;
                if (i < n_values_with_min_bits) {
                    tmp = remaindersBitString(i, min_bits);
                } 
                else {
                    tmp = remaindersBitString(i + n_values_with_min_bits, max_bits);
                }
                remainders.push_back(tmp);
            }
            return remainders;


        }

        // function that converts integer to string of bits with n bits of representation
        std::string remaindersBitString(int num, int n_bits_representation){
            std::string result = "";
            for (int i = 0; i < n_bits_representation; i++) {
                result = std::to_string(num % 2) + result;
                num /= 2;
            }
            return result;
        }

        int bitStringToInt(std::string bit_string) {
            int result = 0;
            for (int i = 0; i < bit_string.length(); i++) {
                result = result * 2 + (bit_string[i] - '0');
            }
            return result;
        }
        

    public:
        // constructor with m as parameter
        Golomb() {
            
            // // print remainders_table
            // for (int i = 0; i < remainders_table.size(); i++) {
            //     std::cout << remainders_table[i] << std::endl;
            // }
        }

        void test(){
            calculateBits(10);
            std::vector<std::string> remainders = calculateRemaindersTable(10);
            for (int i = 0; i < remainders.size(); i++) {
                std::cout << remainders[i] << std::endl;
            }
        }
        
        //decode function
        std::vector<int> decode(std::string encoded_string, int m) {
            //calculate possible remainders given m
            calculateBits(m);
            std::vector<int> result;
            int i = 0;
            while(i < encoded_string.length()) {
                int quotient = 0;
                while (encoded_string[i] == '0') {
                    quotient++;
                    i++;
                }
                i++;
                int remainder = 0;
                //remainder with min_bits
                int j = 0;
                std::string tmp = "";
                while (j < min_bits) {
                    tmp += encoded_string[i];
                    i++;
                    j++;
                }
                int res1 = bitStringToInt(tmp);
                if (res1 < n_values_with_min_bits) {
                    remainder = res1;
                } else {
                    tmp += encoded_string[i];
                    i++;
                    remainder = bitStringToInt(tmp) - n_values_with_min_bits;
                }

                //sign bit
                if (encoded_string[i] == '1') {
                    result.push_back(-(quotient * m + remainder));
                }
                else {
                    result.push_back(quotient * m + remainder);
                }
                i++;
            }
            return result;
        }

        //decode function
        std::vector<int> decodeMultiple(std::string encoded_string, std::vector<int> m_vector, int block_size) {
            std::vector<int> result;
            int i = 0;
            for (int m_i = 0; m_i < m_vector.size(); m_i++) {
                int m = m_vector[m_i];
                int count = 0;
                //calculate possible remainders given m
                calculateBits(m);
                
                while(i < encoded_string.length()) {
                    int quotient = 0;
                    while (encoded_string[i] == '0') {
                        quotient++;
                        i++;
                    }
                    i++;
                    int remainder = 0;
                    //remainder with min_bits
                    int j = 0;
                    std::string tmp = "";
                    while (j < min_bits) {
                        tmp += encoded_string[i];
                        i++;
                        j++;
                    }
                    int res1 = bitStringToInt(tmp);
                    if (res1 < n_values_with_min_bits) {
                        remainder = res1;
                    } else {
                        tmp += encoded_string[i];
                        i++;
                        remainder = bitStringToInt(tmp) - n_values_with_min_bits;
                    }
                    //sign bit
                    int res = quotient * m + remainder;
                    if (encoded_string[i] == '1') {
                        result.push_back(-(res));
                    }
                    else {
                        result.push_back(res);
                    }
                    i++;
                    count++;
                    
                    
                    if (count == block_size) {
                        break;
                    }
                }
            }
            return result;
        }


        std::string encode(int num, int m){
            //calculate possible remainders given m
            calculateBits(m);
            std::string result = "";
            int quotient = abs(num) / m;
            int remainder = abs(num) % m;
            for (int i = 0; i < quotient; i++) {
                result += "0";
            }
            result += "1";
            if (remainder < n_values_with_min_bits) {
                result += remaindersBitString(remainder, min_bits);
            } else {
                result += remaindersBitString(remainder + n_values_with_min_bits, max_bits);
            }
            if (num < 0) {
                result += "1";
            } else {
                result += "0";
            }
            return result;
        }
        
        
        
        // with m bits, we know the number of remainders we can have
        
};