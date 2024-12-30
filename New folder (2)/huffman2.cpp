#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>
#include <string>

using namespace std;

// Node structure for Huffman Tree
struct Node
{
    char ch;
    int freq;
    Node *left;
    Node *right;
    Node(char ch, int freq, Node *left = nullptr, Node *right = nullptr)
        : ch(ch), freq(freq), left(left), right(right) {}
};

// Comparator for priority queue
struct Compare
{
    bool operator()(Node *l, Node *r)
    {
        return l->freq > r->freq;
    }
};

// Global maps for encoding and decoding
unordered_map<char, string> huffmanCode;
unordered_map<string, char> reverseHuffmanCode;

// Helper function to build the Huffman tree
Node *buildHuffmanTree(const vector<char> &data)
{
    unordered_map<char, int> freq;
    for (char ch : data)
    {
        freq[ch]++;
    }
    priority_queue<Node *, vector<Node *>, Compare> pq;
    for (auto &pair : freq)
    {
        pq.push(new Node(pair.first, pair.second));
    }
    while (pq.size() > 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        int sum = left->freq + right->freq;
        pq.push(new Node('\0', sum, left, right));
    }
    return pq.top();
}

// Generate Huffman codes
void generateCodes(Node *root, string str)
{
    if (!root)
        return;
    if (!root->left && !root->right)
    {
        huffmanCode[root->ch] = str;
        reverseHuffmanCode[str] = root->ch;
    }
    generateCodes(root->left, str + "0");
    generateCodes(root->right, str + "1");
}

// Compress the file
void compressFile(const string &inputPath, const string &outputPath)
{
    ifstream inputFile(inputPath, ios::binary);
    if (!inputFile.is_open())
    {
        cerr << "Error opening input file.\n";
        return;
    }
    vector<char> data((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    Node *root = buildHuffmanTree(data);
    generateCodes(root, "");

    string encodedText = "";
    for (char ch : data)
    {
        encodedText += huffmanCode[ch];
    }

    ofstream outputFile(outputPath, ios::binary);
    if (!outputFile.is_open())
    {
        cerr << "Error opening output file.\n";
        return;
    }

    // Save frequency table as metadata
    outputFile << huffmanCode.size() << '\n'; // Number of entries in code map
    for (auto &pair : huffmanCode)
    {
        outputFile << pair.first << ' ' << pair.second << '\n';
    }

    // Write encoded text length
    size_t textLength = encodedText.size();
    outputFile.write(reinterpret_cast<const char *>(&textLength), sizeof(textLength));

    // Write encoded text as bits
    string byteString = "";
    for (char bit : encodedText)
    {
        byteString += bit;
        if (byteString.size() == 8)
        {
            bitset<8> byte(byteString);
            outputFile.put(byte.to_ulong());
            byteString.clear();
        }
    }
    if (!byteString.empty())
    {
        while (byteString.size() < 8)
            byteString += '0';
        bitset<8> byte(byteString);
        outputFile.put(byte.to_ulong());
    }

    outputFile.close();
    cout << "File compressed successfully as: " << outputPath << endl;
}

// Decompress the file
void decompressFile(const string &inputPath, const string &outputPath)
{
    ifstream inputFile(inputPath, ios::binary);
    if (!inputFile.is_open())
    {
        cerr << "Error opening input file.\n";
        return;
    }

    // Read frequency table
    int mapSize;
    inputFile >> mapSize;
    inputFile.ignore(); // Skip newline

    huffmanCode.clear();
    reverseHuffmanCode.clear();

    for (int i = 0; i < mapSize; ++i)
    {
        char ch;
        string code;
        inputFile.get(ch); // Read character
        inputFile >> code;
        inputFile.ignore(); // Skip newline
        huffmanCode[ch] = code;
        reverseHuffmanCode[code] = ch;
    }

    // Read encoded text length
    size_t textLength;
    inputFile.read(reinterpret_cast<char *>(&textLength), sizeof(textLength));

    // Read encoded text as bits
    string encodedText = "";
    char byte;
    while (inputFile.get(byte))
    {
        bitset<8> bits(byte);
        encodedText += bits.to_string();
    }
    encodedText = encodedText.substr(0, textLength); // Trim extra padding
    inputFile.close();

    // Decode the text
    vector<char> decodedData;
    string currentCode = "";
    for (char bit : encodedText)
    {
        currentCode += bit;
        if (reverseHuffmanCode.count(currentCode))
        {
            decodedData.push_back(reverseHuffmanCode[currentCode]);
            currentCode = "";
        }
    }

    ofstream outputFile(outputPath, ios::binary);
    if (!outputFile.is_open())
    {
        cerr << "Error opening output file.\n";
        return;
    }
    outputFile.write(decodedData.data(), decodedData.size());
    outputFile.close();

    cout << "File decompressed successfully as: " << outputPath << endl;
}

// Main menu
int main()
{
    int choice;
    cout << "Huffman Compression Program\n";
    cout << "1. Compress a file\n";
    cout << "2. Decompress a file\n";
    cout << "Enter your choice: ";
    cin >> choice;

    string inputPath, outputPath;
    if (choice == 1)
    {
        cout << "Enter the path of the file to compress: ";
        cin >> inputPath;
        cout << "Enter the name for the compressed file: ";
        cin >> outputPath;
        compressFile(inputPath, outputPath);
    }
    else if (choice == 2)
    {
        cout << "Enter the path of the file to decompress: ";
        cin >> inputPath;
        cout << "Enter the name for the decompressed file: ";
        cin >> outputPath;
        decompressFile(inputPath, outputPath);
    }
    else
    {
        cout << "Invalid choice!\n";
    }
    return 0;
}
