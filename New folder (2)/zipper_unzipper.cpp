#include <iostream>
#include <fstream>
#include <string>
#include <zlib.h>
#include <vector>
#include <zlib.h>

using namespace std;

// Function to compress a file
void compressFile(const string &inputFile, const string &outputFile)
{
    ifstream inFile(inputFile, ios::binary);
    if (!inFile.is_open())
    {
        cerr << "Error opening input file for compression.\n";
        return;
    }

    ofstream outFile(outputFile, ios::binary);
    if (!outFile.is_open())
    {
        cerr << "Error opening output file for compression.\n";
        return;
    }

    // Read the input file into a vector
    vector<char> buffer((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    uLong sourceLen = buffer.size();
    uLong destLen = compressBound(sourceLen); // Max compressed size

    vector<char> compressedData(destLen);

    // Compress the data using zlib
    int result = compress(reinterpret_cast<Bytef *>(compressedData.data()), &destLen, reinterpret_cast<const Bytef *>(buffer.data()), sourceLen);

    if (result != Z_OK)
    {
        cerr << "Error during compression: " << result << endl;
        return;
    }

    outFile.write(compressedData.data(), destLen);

    inFile.close();
    outFile.close();

    cout << "File successfully compressed to: " << outputFile << endl;
}

// Function to decompress a file
void decompressFile(const string &inputFile, const string &outputFile)
{
    ifstream inFile(inputFile, ios::binary);
    if (!inFile.is_open())
    {
        cerr << "Error opening input file for decompression.\n";
        return;
    }

    // Get the size of the compressed file
    inFile.seekg(0, ios::end);
    uLong compressedSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    vector<char> compressedData(compressedSize);
    inFile.read(compressedData.data(), compressedSize);

    // Estimate the decompressed size (we assume it's smaller than the original size)
    uLong destLen = compressedSize * 10; // Safe estimate (adjust if necessary)
    vector<char> decompressedData(destLen);

    // Decompress the data using zlib
    int result = uncompress(reinterpret_cast<Bytef *>(decompressedData.data()), &destLen, reinterpret_cast<const Bytef *>(compressedData.data()), compressedSize);

    if (result != Z_OK)
    {
        cerr << "Error during decompression: " << result << endl;
        return;
    }

    ofstream outFile(outputFile, ios::binary);
    if (!outFile.is_open())
    {
        cerr << "Error opening output file for decompression.\n";
        return;
    }

    outFile.write(decompressedData.data(), destLen);
    inFile.close();
    outFile.close();

    cout << "File successfully decompressed to: " << outputFile << endl;
}

// Main menu for file compression and decompression
int main()
{
    int choice;
    cout << "File Zipper and Unzipper Program\n";
    cout << "1. Compress a file\n";
    cout << "2. Decompress a file\n";
    cout << "Enter your choice: ";
    cin >> choice;

    string inputFile, outputFile;
    if (choice == 1)
    {
        cout << "Enter the path of the file to compress: ";
        cin >> inputFile;
        cout << "Enter the name for the compressed file: ";
        cin >> outputFile;
        compressFile(inputFile, outputFile);
    }
    else if (choice == 2)
    {
        cout << "Enter the path of the file to decompress: ";
        cin >> inputFile;
        cout << "Enter the name for the decompressed file: ";
        cin >> outputFile;
        decompressFile(inputFile, outputFile);
    }
    else
    {
        cout << "Invalid choice!\n";
    }

    return 0;
}
