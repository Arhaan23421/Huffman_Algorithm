
#ifndef HUFFMAN_BIT_H
#define HUFFMAN_BIT_H
#include <ostream>

using namespace std;

/**
 * Type representing a single bit value (0 or 1).  A Bit is largely
 * interchangeable with an int, except that a Bit raises an error
 * if value is anything other than 0 or 1.
 *
 *     Bit zero = 0;
 *
 *     queue.enqueue(0);
 *     queue.enqueue(Bit(1));
 *
 *     if (bit == 0) { ... }
 */
class Bit {
public:
    Bit() = default;
    Bit(int value);

    friend bool operator== (Bit lhs, Bit rhs);
    friend bool operator!= (Bit lhs, Bit rhs);
    friend ostream& operator<< (ostream& out, Bit bit);

private:
    bool value;
};

#endif //HUFFMAN_BIT_H
