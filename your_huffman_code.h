
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
struct CompareTreeNode
{
    bool operator()(const TreeNode* lhs, const TreeNode* rhs) const
    {
        return lhs->weight > rhs->weight;
    }
};
/* You need the unusual CompareTreeNode struct above if you want to make
 * a priority queue of TreeNodes.  (Hint: You do!)  This struct defines an
 * operator for comparing TreeNode*, which makes it possible for the
 * underlying heap for a priority queue to work correctly.  It's weird,
 * but here's the syntax you'll need:
       priority_queue<TreeNode*, vector<TreeNode*>, CompareTreeNode> pq;
 * The first parameter describes what it is a priority queue of, the
 * second parameter describes the underlying heap implementation ("I'm
 * using a vector for this heap"), and the third parameter specifies a
 * way to compare TreeNode*.  Phew...
 */

// NOTE: The struct EncodedData is defined in the huffman_helper.h file

EncodedData compress(string text) {
    EncodedData result;
    // you've got a lot to write here
    return result;
}

string decompress(EncodedData& data) {
    return "I'm not done yet";
}

#endif //HUFFMAN_YOUR_HUFFMAN_CODE_H
