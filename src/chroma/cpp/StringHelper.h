#ifndef STRHELPER_FUNCTIONS
#define STRHELPER_FUNCTIONS

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "Vector3.h"

using namespace std;
using boost::lexical_cast;


inline void strsplit(const string str, const char sep, vector<string> &words) {
    string::size_type a = 0, e;

    while ((a = str.find_first_not_of(sep, a)) != string::npos) {
        e = str.find_first_of(sep, a);
        if (e != std::string::npos) {
            words.push_back(str.substr(a, e - a));
            a = e + 1;
        }
        else {
            words.push_back(str.substr(a));
            break;
        }
    }
}


inline Vector3 strtoVect(string &str) {

    vector<string> vect;            // vector for ,-seperated dimensions of sphere center slot
    strsplit(str, ',', vect);
    if (vect.size() != 3) {
        cout << "parse error in strtoVect: " << str << endl;
        return Vector3(0, 0, 0);
    }

    return Vector3(lexical_cast<float>(vect[0]), lexical_cast<float>(vect[1]), lexical_cast<float>(vect[2]));
}


#endif
