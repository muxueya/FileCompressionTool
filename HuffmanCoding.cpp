#include "HuffmanCoding.h"

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

void HuffmanCoding::encode(Node* root, const string &str) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCode[root->data] = str;
    }

    encode(root->left, str + "0");
    encode(root->right, str + "1");
}

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

void HuffmanCoding::compress(const string &inputFile, const string &outputFile) {
    ifstream inFile(inputFile);
    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    buildHuffmanTree(text);
    encode(root, "");

    string encodedString = "";
    for (char ch : text) {
        encodedString += huffmanCode[ch];
    }

    ofstream outFile(outputFile, ios::binary);
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

void HuffmanCoding::decompress(const string &inputFile, const string &outputFile) {
    ifstream inFile(inputFile, ios::binary);
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
    while (index < (int)encodedString.size() - 1) {
        decode(root, index, encodedString);
    }
    cout << endl;
}