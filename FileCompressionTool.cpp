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
    for (char ch : text) {
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();
        Node *newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    root = pq.top();
}

// Function to encode characters based on the Huffman Tree
void HuffmanCoding::encode(Node* root, const string &str) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCode[root->data] = str;
    }

    encode(root->left, str + "0");
    encode(root->right, str + "1");
}

// Function to decode the encoded string
void HuffmanCoding::decode(Node* root, int &index, const string &str) {
    if (!root) return;

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
    if (!inFile) {
        cerr << "Error: Unable to open input file: " << inputFile << endl;
        exit(EXIT_FAILURE);
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
    if (!outFile) {
        cerr << "Error: Unable to open output file: " << outputFile << endl;
        exit(EXIT_FAILURE);
    }

    bitset<8> bits;
    for (size_t i = 0; i < encodedString.size(); i++) {
        bits[i % 8] = encodedString[i] - '0';
        if (i % 8 == 7) {
            outFile.put(static_cast<char>(bits.to_ulong()));
            bits.reset();
        }
    }

    if (encodedString.size() % 8 != 0) {
        outFile.put(static_cast<char>(bits.to_ulong()));
    }

    outFile.close();
}

// Function to decompress the encoded file
void HuffmanCoding::decompress(const string &inputFile, const string &outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "Error: Unable to open input file for decompression: " << inputFile << endl;
        exit(EXIT_FAILURE);
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

// Function to display the menu
void displayMenu() {
    cout << "\nHuffman Coding Tool Menu\n";
    cout << "1. Compress a file\n";
    cout << "2. Decompress a file\n";
    cout << "3. Exit\n";
    cout << "Choose an option: ";
}

// Function to get user input with error handling
int getUserChoice() {
    int choice;
    while (true) {
        cin >> choice;
        if (cin.fail()) { // Check for invalid input
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter a number (1-3): ";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the buffer
            if (choice >= 1 && choice <= 3) {
                return choice; // Valid choice
            } else {
                cout << "Invalid choice. Please enter a number between 1 and 3: ";
            }
        }
    }
}

int main() {
    HuffmanCoding huffman;
    int choice;

    while (true) {
        displayMenu();
        choice = getUserChoice(); // Use the function to get user input

        switch (choice) {
            case 1: {
                string inputFile, compressedFile;
                cout << "Enter the path to the input file for compression: ";
                cin >> inputFile;
                compressedFile = "compressed.bin"; // Default output file name
                huffman.compress(inputFile, compressedFile);
                cout << "File compressed successfully to " << compressedFile << "." << endl;
                break;
            }
            case 2: {
                string compressedFile, decompressedFile;
                cout << "Enter the path to the compressed file: ";
                cin >> compressedFile;
                decompressedFile = "decompressed.txt"; // Default decompressed output file name
                huffman.decompress(compressedFile, decompressedFile);
                cout << "File decompressed successfully. Output is shown in the console." << endl;
                break;
            }
            case 3:
                cout << "Exiting the program." << endl;
                exit(EXIT_SUCCESS);
            default:
                cout << "This should not happen! Please try again." << endl;
        }
    }

    return 0;
}