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
        Golomb(int m) {
            this->m = m;
            calculateBits();
            remainders_table = calculateRemaindersTable();
            // // print remainders_table
            // for (int i = 0; i < remainders_table.size(); i++) {
            //     std::cout << remainders_table[i] << std::endl;
            // }
        }
        // implement even-odd implementation and compare results
        // function that converts string of bits to integer
        std::vector<int> bitStringToIntVector(std::string bitString) {
            std::vector<int> results;
            int result = 0;
            int countZeros = 0;
            bool countingZeros = true;
            int remainder = 0;
            bool countingRemainder = false;
            bool readingSignal = false;
            std::string tmp = "";
            for (int i = 0; i < bitString.length(); i++) {
                if (!countingZeros && !readingSignal) {
                    tmp += bitString[i];
                    //check if tmp is in remainders_table
                    for (int j = 0; j < remainders_table.size(); j++) {
                        if (tmp == remainders_table[j]) {
                            remainder = j;
                            tmp = "";
                            countingRemainder = false;
                            readingSignal = true;
                            if (remainder != 0 || countZeros != 0) i++;
                            
                            //leave for loop
                            break;
                        }
                    }
                }
                if ((bitString[i] == '0') && countingZeros && !readingSignal) {
                    countZeros++;
                }
                if ((bitString[i] == '1') && countingZeros && !countingRemainder && !readingSignal) {
                    countingZeros = false;
                    countingRemainder = true;
                }

                if(readingSignal) {
                    if (bitString[i] == '1'){
                        result = ((countZeros * m) + remainder )* (-1);
                        countingZeros = true;

                    }
                    else {
                        result = (countZeros * m) + remainder;
                        countingZeros = true;
                    }
                    results.push_back(result);
                    result = 0;
                    countZeros = 0;
                    remainder = 0;
                    readingSignal = false;
                }
                

                    
            }
            return results;
        }

        std::string intToBitString(int num){
            //keep the original_number saved for later
            int original_number = num;
            if (num < 0){
                num = -num;
            }
            int quotient = num / m;
            int remainder = num % m;
            std::string result = "";
            for (int i = 0; i < quotient; i++) {
                result += "0";
            }
            result += "1";
            //add the remainder from the table
            result += remainders_table[remainder];
            //add the signal for negative numbers after remainder (original_number is different from num)
            if (num != 0){
                if (original_number != num) {
                    result = result + "1";
                } else {
                    result = result + "0";
                }
            }
            return result;
        }
        
        
        
        // with m bits, we know the number of remainders we can have
        
};
