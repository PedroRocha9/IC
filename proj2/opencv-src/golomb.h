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
            if (m != 0){
                max_bits = ceil(log2(m));
                min_bits = max_bits - 1;
                n_values_with_min_bits = pow(2, max_bits) - m;
            } else {
                max_bits = 0;
                min_bits = 0;
                n_values_with_min_bits = 0;
            }
        }

        // return array of strings based on golomb
        std::vector<std::string> calculateRemaindersTable(int m){
            std::vector<std::string> remainders;
            for (int i = 0; i < m; i++) {
                std::string tmp;
                if (i < n_values_with_min_bits) {
                    tmp = remaindersBitString(i, min_bits);
                } else {
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
        Golomb() { }

        void test() {
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

                if (m != 1){
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
                } else {
                    remainder = 0;
                    i++;
                }
                // std::cout << remainder << " r---" << std::endl;

                //sign bit
                int res = quotient * m + remainder;
                if (encoded_string[i] == '1') {
                    result.push_back(-(res));
                } else {
                    result.push_back(res);
                }

                i++;
            }

            return result;
        }

        //decode function
        std::vector<int> decodeMultiple(std::string encoded_string, std::vector<int> m_vector, int block_size) {
            std::vector<int> result;
            int i = 0;
            int m_i = 0;
            int count = 0;
            //calculate possible remainders given m
            calculateBits(m_vector[m_i]);
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

                    if (m_vector[m_i] != 1){
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
                    } else {
                        remainder = 0;
                        i++;
                    }
                    // std::cout << remainder << " r---" << std::endl;

                    //sign bit
                    int res = quotient * m_vector[m_i] + remainder;
                    if (encoded_string[i] == '1') {
                        result.push_back(-(res));
                    } else {
                        result.push_back(res);
                    }

                    i++;
                    count++;
                    
                    if (count == block_size) {
                        m_i++;
                        count = 0;
                        calculateBits(m_vector[m_i]);
                    }  
                }
            return result;
        }

        std::string encode(int num, int m){
            //calculate possible remainders given m
            calculateBits(m);
            std::string result = "";
            int quotient = 0;
            int remainder = 0;
            if (m != 0){
                quotient = abs(num) / m;
                remainder = abs(num) % m;
            }
            for (int i = 0; i < quotient; i++) {
                result += "0";
            }

            result += "1";
            if (m != 1){
                if (remainder < n_values_with_min_bits) {
                    result += remaindersBitString(remainder, min_bits);
                } else {
                    result += remaindersBitString(remainder + n_values_with_min_bits, max_bits);
                }
            }else{
                result += "0";
            }

            num < 0 ? result += "1" : result += "0";


            return result;
        }
};
