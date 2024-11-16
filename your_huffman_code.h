#ifndef HUFFMAN_YOUR_HUFFMAN_CODE_H
#define HUFFMAN_YOUR_HUFFMAN_CODE_H
#include <map>
#include "huffman_helper.h"
using namespace std;
struct TreeNode {
    char ch;
    int weight;
    TreeNode *left;
    TreeNode *right;
};
struct CompareTreeNode{
    bool operator()(const TreeNode* lhs, const TreeNode* rhs) const
    {
        return lhs->weight > rhs->weight;
    }
};
/* You need the unusual CompareTreeNode struct above if you want to make
* a priority queue of TreeNodes. (Hint: You do!) This struct defines an * operator for comparing TreeNode*, which makes it possible
for the * underlying heap for a priority queue to work correctly. It's weird, * but here's the syntax you'll need:
priority_queue<TreeNode*, vector<TreeNode*>, CompareTreeNode> pq; * The first parameter describes what it is a priority queue of, the
* second parameter describes the underlying heap implementation ("I'm * using a vector for this heap"), and the third parameter
specifies a * way to compare TreeNode*. Phew... */
// NOTE: The struct EncodedData is defined in the huffman_helper.h file

void deleteTree(TreeNode* root){
    if(root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

void buildingCodeMap(TreeNode* root, map<char, queue<Bit>> &paths, queue<Bit> currPath){
    if(root->left == nullptr && root->right == nullptr){
        paths[root->ch] = currPath;
        return;
    }
        queue<Bit> leftPath = currPath;
        leftPath.push(0);
        buildingCodeMap(root->left, paths, leftPath);

        queue<Bit> rightPath = currPath;
        rightPath.push(1);
        buildingCodeMap(root->right, paths, rightPath);

}
void putTreeIntoData(EncodedData &data, TreeNode* root){
    if(root->left == nullptr && root->right == nullptr){
        data.treeShape.push(0);
        data.treeLeaves.push(root->ch);
        return;
    } else {
        data.treeShape.push(1);
        putTreeIntoData(data, root->left);
        putTreeIntoData(data, root->right);
    }
}
priority_queue<TreeNode *, vector<TreeNode *>, CompareTreeNode> mYPriorityQueue(map<char, int> &freq) {
    priority_queue<TreeNode*, vector<TreeNode*>, CompareTreeNode> sample;
    for (auto it = freq.begin(); it != freq.end(); it++) {
        TreeNode* node = new TreeNode();
        node->ch = it->first;
        node->weight = it->second;
        sample.push(node);
    }
    return sample;
}
TreeNode* createTree(map<char, int> freq) {
    priority_queue<TreeNode*, vector<TreeNode*>, CompareTreeNode> sample = mYPriorityQueue(freq);

    while (sample.size() > 1) {
        TreeNode* left = sample.top();
        sample.pop();
        TreeNode* right = sample.top();
        sample.pop();

        TreeNode* parent = new TreeNode();
        parent->left = left;
        parent->right = right;
        parent->weight = left->weight + right->weight;

        sample.push(parent);
    }
    return sample.top();
}

EncodedData encode(map<char, queue<Bit>> &codingMap, const string &text, TreeNode* &root){
    queue<Bit> currcode;
    for(char c : text){
        queue<Bit> q = codingMap[c];
        while(!q.empty()){
            currcode.push(q.front());
            q.pop();
        }
    }
    EncodedData data;
    data.messageBits = currcode;
    putTreeIntoData(data, root);
    return data;
}



map<char, int> makeFreqMap(const string &text) {
    map<char, int> freq;
    for (char ch : text)
        freq[ch]++;
    return freq;
}




EncodedData compress(const string &text) {
    map<char, int> freq = makeFreqMap(text);
    TreeNode* root = createTree(freq);
    map<char, queue<Bit>> code;
    buildingCodeMap(root, code, queue<Bit>());
    EncodedData temp = encode(code, text, root);
    deleteTree(root);
    return temp;
}

TreeNode* loadEncodedTree(EncodedData& data){
    if(data.treeLeaves.empty()) {
        return nullptr;
    }
    TreeNode* root = new TreeNode;
    if(data.treeShape.front() == 0) {
        data.treeShape.pop();
        root->ch = data.treeLeaves.front();
        root->left = nullptr;
        root->right = nullptr;
        data.treeLeaves.pop();
    } else {
        data.treeShape.pop();
        root->left = loadEncodedTree(data);
        root->right = loadEncodedTree(data);
    }
    return root;
}
string decompress(EncodedData& data) {
    TreeNode* root = loadEncodedTree(data);
    string result = "";
    while(!data.messageBits.empty()){
        TreeNode* current = root;
        while(current->left != nullptr && current->right != nullptr){
            if(data.messageBits.front() == 0)
            current = current->left;
            else
            current = current->right;
            data.messageBits.pop();
        }
        result += current->ch;
    }
    return result;
}
#endif //HUFFMAN_YOUR_HUFFMAN_CODE_H