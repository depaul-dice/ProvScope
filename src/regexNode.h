#ifndef REGEXNODE_H
#define REGEXNODE_H

#include "regexGraph.h"
#include "cfg.h"
#include <cassert>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <tuple>

class regexNode
{
public:
    regexNode() = delete;
    regexNode(const regexNode&);
    regexNode& operator = (const regexNode&) = delete;
    ~regexNode() = default;

    regexNode(node *content, int index, std::set<int> colors); // this is when it actually have the node *
    regexNode(std::set<int> color, std::vector<std::vector<regexNode>> nodes); // this is when it is virtual node
    
    // below are all necessary for the map implementation as well as direct comparison
    bool operator == (const regexNode&) const; // operator overloading for ==
    bool operator != (const regexNode &rhs) const;
    bool operator < (const regexNode&) const; // this is necessary for map implementation

    node *get();
    bool isVirtual();
    unsigned getIndex();
    std::vector<std::vector<regexNode>> getInside();

    friend std::ostream& operator << (std::ostream& os, const regexNode& tmp);

private:
    bool _isVirtual;
    std::set<int> colors;
    node *content; // nullptr if it is a virtual node
    unsigned originalIndex;
    std::vector<std::vector<regexNode>> nodes; 
};

namespace regex {

    std::vector<regexNode> pathAnalysis(std::vector<node *> path, regexGraph& rg, std::set<int> &baseColors, unsigned &baseIndex);

    void pathPrint(std::vector<regexNode> &path);

    void regexRealignment(std::map<regexNode, std::vector<unsigned>> &realignmentMap, std::vector<regexNode> &path1, std::vector<regexNode> &path2, unsigned &i, unsigned &j, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, std::vector<std::tuple<unsigned, unsigned>> &alignedVirtualNodes, std::vector<unsigned> &convergencePoints);

    void prepareMap(std::map<regexNode, std::vector<unsigned>> &realignmentMap, std::vector<regexNode> &path1, int i);
    void regexRealignment(std::map<regexNode, int> &realignmentMap, std::vector<regexNode> &path1, std::vector<regexNode> &path2, int &i, int &j, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs);
    void regexAlignment(std::vector<regexNode> &path1, std::vector<regexNode> &path2, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff);

}
#endif /* REGEXNODE_H */
