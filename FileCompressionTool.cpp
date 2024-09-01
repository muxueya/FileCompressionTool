#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <bitset>
#include <string>
#include <cstdlib> // For exit()

using namespace std;

struct Node {
    char data;
    int freq;
    Node *left, *right;

    Node(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

class HuffmanCoding {
public:
    void buildHuffmanTree(const string &text);
    void encode(Node* root, const string &str);
    void decode(Node* root, int &index, const string &str);
    void compress(const string &inputFile, const string &outputFile);
    void decompress(const string &inputFile, const string &outputFile);

private:
    Node* root;
    unordered_map<char, string> huffmanCode;
};

// Function to build the Huffman Tree from the input text
void HuffmanCoding::buildHuffmanTree(const string &text) {
    unordered_map<char, int> freq;
    
    // Count frequency of each character in the text
    for (char ch : text) {
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, Compare> pq;
    
    // Create a priority queue of nodes
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Build the Huffman Tree
    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();
        Node *newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    root = pq.top(); // Root of the Huffman Tree
}

// Function to encode characters based on the Huffman Tree
void HuffmanCoding::encode(Node* root, const string &str) {
    if (!root) return;

    // Store the code for leaf nodes
    if (!root->left && !root->right) {
        huffmanCode[root->data] = str;
    }

    encode(root->left, str + "0");
    encode(root->right, str + "1");
}

// Function to decode the encoded string
void HuffmanCoding::decode(Node* root, int &index, const string &str) {
    if (!root) return;

    // If leaf node, print the character
    if (!root->left && !root->right) {
        cout << root->data;
        return;
    }

    index++;
    if (str[index] == '0') {
        decode(root->left, index, str);
    } else {
        decode(root->right, index, str);
    }
}

// Function to compress the input file
void HuffmanCoding::compress(const string &inputFile, const string &outputFile) {
    ifstream inFile(inputFile);
    
    // Error handling for file opening
    if (!inFile) {
        cerr << "Error: Unable to open input file: " << inputFile << endl;
        exit(EXIT_FAILURE); // Exit the program if the file can't be opened
    }
    
    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    buildHuffmanTree(text);
    encode(root, "");

    string encodedString = "";
    for (char ch : text) {
        encodedString += huffmanCode[ch];
    }

    ofstream outFile(outputFile, ios::binary);
    
    // Error handling for file writing
    if (!outFile) {
        cerr << "Error: Unable to open output file: " << outputFile << endl;
        exit(EXIT_FAILURE); // Exit the program if the output file can't be opened
    }

    bitset<8> bits;
    for (size_t i = 0; i < encodedString.size(); i++) {
        bits[i % 8] = encodedString[i] - '0';
        if (i % 8 == 7) {
            outFile.put(static_cast<char>(bits.to_ulong()));
            bits.reset();
        }
    }

    // Handle any remaining bits
    if (encodedString.size() % 8 != 0) {
        outFile.put(static_cast<char>(bits.to_ulong()));
    }

    outFile.close();
}

// Function to decompress the encoded file
void HuffmanCoding::decompress(const string &inputFile, const string &outputFile) {
    ifstream inFile(inputFile, ios::binary);
    
    // Error handling for file opening
    if (!inFile) {
        cerr << "Error: Unable to open input file for decompression: " << inputFile << endl;
        exit(EXIT_FAILURE); // Exit the program if the input file can't be opened
    }

    string encodedString;
    char byte;
    while (inFile.get(byte)) {
        bitset<8> bits(byte);
        for (int i = 0; i < 8; i++) {
            encodedString += bits[i] ? '1' : '0';
        }
    }
    inFile.close();

    int index = -1;
    cout << "Decompressed output: ";
    while (index < (int)encodedString.size() - 1) {
        decode(root, index, encodedString);
    }
    cout << endl;
}

int main() {
    HuffmanCoding huffman;
    string inputFile, compressedFile, decompressedFile;

    cout << "Enter the path to the input file for compression: ";
    cin >> inputFile;
    compressedFile = "compressed.bin"; // Default output file name
    decompressedFile = "decompressed.txt"; // Default decompressed output file name

    // Compress the input file
    huffman.compress(inputFile, compressedFile);
    cout << "File compressed successfully to " << compressedFile << "." << endl;

    // Decompress the compressed file
    huffman.decompress(compressedFile, decompressedFile);
    cout << "File decompressed successfully. Output is shown in the console." << endl;

    return 0;
}