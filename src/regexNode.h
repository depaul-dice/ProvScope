#ifndef REGEXNODE_H
#define REGEXNODE_H

#include "regexGraph.h"
#include "cfg.h"
#include <cassert>
#include <vector>
#include <set>

class regexNode
{
public:
    regexNode() = delete;
    regexNode(const regexNode&);
    regexNode& operator = (const regexNode&) = delete;
    ~regexNode() = default;

    regexNode(node *content, int index, std::set<int> colors); // this is when it actually have the node *
    regexNode(std::set<int> color, std::vector<std::vector<regexNode>> nodes); // this is when it is virtual node
    node *get();

    friend std::ostream& operator << (std::ostream& os, const regexNode& tmp);

private:
    bool isVirtual;
    std::set<int> colors;
    node *content; // nullptr if it is a virtual node
    int originalIndex;
    std::vector<std::vector<regexNode>> nodes; 
};

namespace regex {

    std::vector<regexNode> pathAnalysis(std::vector<node *> path, regexGraph& rg, std::set<int> &baseColors, unsigned &baseIndex);

    void pathPrint(std::vector<regexNode> &path);

}
#endif /* REGEXNODE_H */
