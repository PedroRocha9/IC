#include "golomb.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    Golomb g;
    vector<int> v = {1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1};
    vector<int> m = {1, 1, 1};
    int bs = 5;

    string encoded = "";
    string tmp = "";
    int m_index = 0;
    for (int i = 0; i < v.size(); i++) {
        if (i % bs == 0 && i != 0) {
            m_index++;
            if (m_index >= m.size()) {
                break;
            }
        }
        tmp = g.encode(v[i], m[m_index]);
        encoded += tmp;
    }

    //decode
    vector<int> decoded = g.decodeMultiple(encoded, m, bs);

    for (int i = 0; i < decoded.size(); i++) {
        cout << decoded[i] << endl;
    }

   
    return 0;
}