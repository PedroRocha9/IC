#include "golomb.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    Golomb g(5);
    string test1 = g.intToBitString(28);
    string test2 = g.intToBitString(0);
    string test3 = g.intToBitString(6);
    string test4 = g.intToBitString(7);
    string test5 = g.intToBitString(5);
    string test6 = g.intToBitString(4);
    string test7 = g.intToBitString(3);
    string test8 = g.intToBitString(0);
    string test9 = g.intToBitString(2);
    string test10 = g.intToBitString(10);
    string test11 = g.intToBitString(11);
    string test12 = g.intToBitString(1);
    string test13 = g.intToBitString(13);
    string test14 = g.intToBitString(14);
    string test15 = g.intToBitString(-16);
    string test16 = g.intToBitString(4);
    string test17 = g.intToBitString(-5);
    string test18 = g.intToBitString(-7);
    string test19 = g.intToBitString(1);
    string test20 = g.intToBitString(-70);
    string test21 = g.intToBitString(0);
    string test22 = g.intToBitString(-98);

    string total = test1 + test2 + test3 + test4 + test5 + test6 + test7 + test8 + test9 + test10 + test11 + test12 + test13 + test14 + test15 + test16 + test17 + test18 + test19 + test20 + test21 + test22;
    cout << total << endl;
    cout << "num bits: " << total.length() << endl;
    vector<int> decoded = g.bitStringToIntVector(total);
    cout << "---------------------" << endl;
    for (int i = 0; i < decoded.size(); i++) {
        cout << decoded[i] << endl;
    }


    return 0;
}