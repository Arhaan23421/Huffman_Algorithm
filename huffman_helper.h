
#ifndef HUFFMAN_HUFFMAN_HELPER_H
#define HUFFMAN_HUFFMAN_HELPER_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <queue>
#include "Bit.h"
using namespace std;


struct EncodedData {
    queue<Bit>  treeShape;
    queue<char> treeLeaves;
    queue<Bit>  messageBits;
};

string directoryPathSeparator = "/";

bool fileExists(const string &fileName)
{
    struct stat buffer;
    return (stat (fileName.c_str(), &buffer) == 0);
}

int getFileSize(const string &fileName) {
    ifstream inFile(fileName, ios::binary);
    inFile.seekg(0, ios::end);
    int fileSize = inFile.tellg();
    inFile.close();
    return fileSize;
}

string getHead(const string& filename) {
    size_t slash = string::npos;
    size_t len = filename.length();
    for (size_t i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '/' || ch == '\\') slash = i;
    }
    if (slash == string::npos) {
        return "";
    } else if (slash == 0) {
        return "/";
    } else {
        return filename.substr(0, slash);
    }
}
std::string getTail(const string& filename) {
    size_t slash = string::npos;
    size_t len = filename.length();
    for (size_t i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '/' || ch == '\\') slash = i;
    }
    if (slash == string::npos) {
        return filename;
    } else {
        return filename.substr(slash + 1);
    }
}

string getRoot(const string& filename) {
    int dot = -1;
    int len = filename.length();
    for (int i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '.') dot = i;
        if (ch == '/' || ch == '\\') dot = -1;
    }
    if (dot == -1) {
        return filename;
    } else {
        return filename.substr(0, dot);
    }
}

void checkIntegrityOf(const EncodedData& data) {
    /* Number of distinct characters must be at least two. */
    if (data.treeLeaves.size() < 2) {
        cerr << "File must contain at least two distinct characters." << endl;
    }

    /* Number of bits in tree shape should be exactly 2c - 1, where c is the number of
     * distinct characters.
     */
    if (data.treeShape.size() != data.treeLeaves.size() * 2 - 1) {
        cerr << "Wrong number of tree bits for the given leaves." << endl;
    }
}

/* Utility types for reading/writing individual bits. Inspired by a
 * similar implementation by Julie Zelenski.
 */
class BitWriter {
public:
    explicit BitWriter(ostream& o) : _out(o) {}
    ~BitWriter() {
        if (_bitIndex != 0) flush();
    }

    void put(Bit b) {
        if (b != 0) {
            _bitBuffer |= (1U << _bitIndex);
        }

        _bitIndex++;
        if (_bitIndex == 8) {
            flush();
        }
    }

private:
    void flush() {
        _out.put(_bitBuffer);
        _bitBuffer = 0;
        _bitIndex = 0;
    }

    ostream& _out;
    uint8_t _bitBuffer = 0;
    uint8_t _bitIndex  = 0;
};

class BitReader {
public:
    explicit BitReader(istream& i) : _in(i) {}

    Bit get() {
        if (_bitIndex == 8) readMore();

        Bit result = !!(_bitBuffer & (1U << _bitIndex));
        _bitIndex++;
        return result;
    }

private:
    istream& _in;
    uint8_t _bitBuffer = 0;
    uint8_t _bitIndex  = 8;

    void readMore() {
        char read;
        if (!_in.get(read)) {
            cerr << "Unexpected end of file when reading bits." << endl;
        }

        _bitBuffer = read;
        _bitIndex = 0;
    }
};

/* "CS106B A7" */
const uint32_t kFileHeader = 0xC5106BA7;

/**
 * We store EncodedData on disk as follows:
 *
 *
 * 1 byte:  number of distinct characters, minus one.
 * c bytes: the leaves of the tree, in order.
 * 1 byte:  number of valid bits in the last byte.
 * n bits:  tree bits, followed by message bits.
 *
 * We don't need to store how many bits are in the tree, since it's always given
 * by 2*c - 1, as this is the number of nodes in a full binary tree with c leaves.
 */
void writeData(EncodedData& data, ostream& out) {
    /* Validate invariants. */
    checkIntegrityOf(data);

    /* Write magic header. */
    out.write(reinterpret_cast<const char *>(&kFileHeader), sizeof kFileHeader);

    /* Number of characters. */
    const uint8_t charByte = data.treeLeaves.size() - 1;
    out.put(charByte);

    /* Tree leaves. */
    while (!data.treeLeaves.empty())  {
        out.put(data.treeLeaves.front());
        data.treeLeaves.pop();
    }

    /* Number of bits in the last byte to read. */
    uint8_t modulus = (data.treeShape.size() + data.messageBits.size()) % 8;
    if (modulus == 0) modulus = 8;
    out.put(modulus);

    /* Bits themselves. */
    BitWriter writer(out);
    while (!data.treeShape.empty()) {
        writer.put(data.treeShape.front());
        data.treeShape.pop();
    }
    while (!data.messageBits.empty()) {
        writer.put(data.messageBits.front());
        data.messageBits.pop();
    }
}

/**
 * Reads EncodedData from stream.
 */
EncodedData readData(istream& in) {
    /* Read back the magic header and make sure it matches. */
    uint32_t header;
    if (!in.read(reinterpret_cast<char *>(&header), sizeof header) ||
        header != kFileHeader) {
        cerr << "Chosen file is not a Huffman-compressed file." << endl;
    }

    EncodedData data;

    /* Read the character count. */
    char skewCharCount;
    if (!in.get(skewCharCount)) {
        cerr << "Error reading character count." << endl;
    }

    /* We offset this by one - add the one back. */
    int charCount = uint8_t(skewCharCount);
    charCount++;

    if (charCount < 2) {
        cerr << "Character count is too low for this to be a valid file." << endl;
    }

    /* Read in the leaves. */
    vector<char> leaves(charCount);
    if (!in.read(leaves.data(), leaves.size())) {
        cerr << "Could not read in all tree leaves." << endl;
    }
    for (char leaf: leaves) {
        data.treeLeaves.push(leaf);
    }

    /* Read in the modulus. */
    char signedModulus;
    if (!in.get(signedModulus)) {
        cerr << "Error reading modulus." << endl;
    }
    uint8_t modulus = signedModulus;

    /* See how many bits we need to read. To do this, jump to the end of the file
     * and back to where we are to count the bytes, then transform that to a number
     * of bits.
     *
     * Thanks to Julie Zelenski for coming up with this technique!
     */
    auto currPos = in.tellg();
    if (!in.seekg(0, istream::end)) {
        cerr << "Error seeking to end of file." << endl;
    }
    auto endPos  = in.tellg();
    if (!in.seekg(currPos, istream::beg)) {
        cerr << "Error seeking back to middle of file." << endl;
    }

    /* Number of bits to read = (#bytes - 1) * 8 + modulus. */
    uint64_t bitsToRead = (endPos - currPos - 1) * 8 + modulus;

    /* Read in the tree shape bits. */
    BitReader reader(in);
    for (int i = 0; i < 2 * charCount - 1; i++) {
        data.treeShape.push(reader.get());
        bitsToRead--;
    }

    /* Read in the message bits. */
    while (bitsToRead > 0) {
        data.messageBits.push(reader.get());
        bitsToRead--;
    }

    return data;
}
#endif //HUFFMAN_HUFFMAN_HELPER_H
