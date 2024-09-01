#ifndef HUFFMANCODING_H
#define HUFFMANCODING_H

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <bitset>
#include <string>

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

#endif