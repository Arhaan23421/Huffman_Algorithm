#include <iostream>
#include <fstream>
#include <cctype>
#include <queue>
#include "Bit.h"
#include "huffman_helper.h"
#include "your_huffman_code.h"
using namespace std;

const string kCompressedExtension = ".huf";
const string kDecompressedExtension = "unhuf.";


void intro() {
    cout << "Welcome to the ATCS Huffman Encoder/Decoder!" << endl;
    cout << "This program uses the Huffman coding algorithm for compression." << endl;
    cout << "Any type of file can be encoded using a Huffman code." << endl;
    cout << "Decompressing the result will faithfully reproduce the original." << endl;
}

char menu() {
    cout << endl;
    cout << "Your options are:" << endl;
    cout << "C) compress file" << endl;
    cout << "D) decompress file" << endl;
    cout << "Q) quit" << endl;

    cout << endl;
    cout << "Enter your choice: ";
    string str;
    getline(cin, str);
    char result = str[0];
    return toupper(result);
}

bool getInputAndOutputFiles(string& inFilename, string& outFilename, bool compressing)
{
    cout << "Input file name: ";
    getline(cin, inFilename);
    bool probablyNeedToAddFilePath = false;
    while (!fileExists(inFilename)) {
        string checkThisFile = "files/" + inFilename;
        if (fileExists(checkThisFile)) {
            inFilename = checkThisFile;
            probablyNeedToAddFilePath = true;
            break;
        }
        cout << "No file found with that name.  Try again." << endl;
        cout << "Input file name: ";
        getline(cin, inFilename);
    }
    string defaultName;
    if (compressing) {
        defaultName = inFilename + kCompressedExtension;
    } else {
        string head = getHead(inFilename);
        string tail = getTail(inFilename);
        defaultName = (!head.empty()? head + directoryPathSeparator : "") + kDecompressedExtension + getRoot(tail);
    }
    cout << "Output file name (Enter for " << defaultName << "):";
    getline(cin, outFilename);

    if (outFilename.empty()) {
        outFilename = defaultName;
    }
    if (inFilename == outFilename) {
        cout << "You cannot specify the same filename as both the input file" << endl;
        cout << "and the output file.  Canceling operation." << endl;
        return false;
    }
    if (probablyNeedToAddFilePath && outFilename.substr(0, 6) != "files/")
        outFilename = "files/" + outFilename;
    if (fileExists(outFilename)) {
        cout << outFilename + " already exists. Overwrite? (Y/N) ";
        string str;
        getline(cin, str);
        return !str.empty() && toupper(str[0]) == 'Y';
    }
    return true;
}

string readEntireBinaryFile(string filename)
{
    ifstream in(filename, std::ios::binary);
    string str;
    str.assign((std::istreambuf_iterator<char>(in)),
               std::istreambuf_iterator<char>());
    return str;
}

void writeEntireBinaryFile(string filename, string data)
{
    ofstream outFile(filename, std::ios::binary);
    outFile.write(data.c_str(), data.size());
}

/*
 * Compress a file.
 * Prompts for input/output file names and opens streams on those files.
 * Then calls your compress function and displays information about size of
 * compressed output.
 */
void compressFile() {
    string inFilename, outFilename;

    if (!getInputAndOutputFiles(inFilename, outFilename, true)) {
        return;
    }
    cout << "Reading " << getFileSize(inFilename) << " input bytes." << endl;
    try {
        string text = readEntireBinaryFile(inFilename);
        cout << "Compressing ..." << endl;
        EncodedData data = compress(text);
        ofstream outFile(outFilename, ios::binary);
        writeData(data, outFile);
    } catch (exception& e) {
        cout << "Ooops! " << e.what() << endl;
    }

    if (fileExists(outFilename)) {
        cout << "Wrote " << getFileSize(outFilename) << " compressed bytes." << endl;
    } else {
        cout << "Compressed output file was not found; perhaps there was an error." << endl;
    }
}

void decompressFile() {
    string inFilename, outFilename;

    if (!getInputAndOutputFiles(inFilename, outFilename, false)) {
        return;
    }
    cout << "Reading " << getFileSize(inFilename) << " input bytes." << endl;
    try {
        ifstream input(inFilename, ios::binary);
        EncodedData data = readData(input);
        cout << "Decompressing ..." << endl;
        string text = decompress(data);
        writeEntireBinaryFile(outFilename, text);
    } catch (exception& e) {
        cerr << "Ooops! " << e.what() << endl;
    }

    if (fileExists(outFilename)) {
        cout << "Wrote " << getFileSize(outFilename) << " decompressed bytes." << endl;
    } else {
        cout << "Decompressed output file was not found; perhaps there was an error." << endl;
    }
}

int main() {
    intro();
    while (true) {
        char choice = menu();
        if (choice == 'Q') {
            break;
        } else if (choice == 'C') {
            compressFile();
        } else if (choice == 'D') {
            decompressFile();
        }
    }
    return 0;
}