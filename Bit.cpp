
#include "Bit.h"
#include <iostream>

using namespace std;

Bit::Bit(int value) {
    /* Check for use of chararacter values. */
    if (value == '0' || value == '1') {
        cerr << "You have attempted to create a bit equal to the character '0' or '1'. " <<
             "The characters '0' and '1' are not the same as the numbers 0 and 1. " <<
             "Edit your code to instead use the numeric values 0 and 1 instead." << endl;
    }
    if (value != 0 && value != 1) {
        cerr << "Illegal value for a bit: " <<  to_string(value) << endl;
    }

    this->value = (value == 1);
}

bool operator== (Bit lhs, Bit rhs) {
    return lhs.value == rhs.value;
}
bool operator!= (Bit lhs, Bit rhs) {
    return !(lhs == rhs);
}
ostream& operator<< (ostream& out, Bit bit) {
    char result;
    if (bit.value)
        result = '1';
    else
        result = '0';
    return out << result;
}
