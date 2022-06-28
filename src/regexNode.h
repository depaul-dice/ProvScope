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

/*
 * This is a class that node could be virtual containing a vector of vector of nodes (each vector representing the one iteration of the loop)
 * This class is used as a set with regexGraph, which does the preliminary stuff to coloring the nodes assigning which nodes belong to which loop.
 */ 
class regexNode
{
public:
    regexNode() = delete;
    regexNode(const regexNode&);
    regexNode& operator = (const regexNode&) = delete;
    ~regexNode() = default;

    regexNode(node *content, int index, std::set<int> colors); // this is when it actually have the node *
    regexNode(std::set<int> color, node *startNode, std::vector<std::vector<regexNode>> nodes); // this is when it is virtual node
    
    // below are all necessary for the map implementation as well as direct comparison
    bool operator == (const regexNode&) const; // operator overloading for ==
    bool operator != (const regexNode &rhs) const;
    bool operator < (const regexNode&) const; // this is necessary for map implementation

    node *get();
    bool isVirtual();
    unsigned getIndex();
    std::vector<std::vector<regexNode>> getInside();
    void setIndex(unsigned index);

    friend std::ostream& operator << (std::ostream& os, const regexNode& tmp);

private:
    bool _isVirtual;
    std::set<int> colors;
    node *content; // nullptr if it is a virtual node
    unsigned originalIndex;
    std::vector<std::vector<regexNode>> nodes; 
};

class Interval
{
public:
    Interval() = default;
    Interval(const Interval&) = default;
    Interval& operator = (const Interval&) = default;
    ~Interval() = default;
    Interval(unsigned start, unsigned end);

    friend std::ostream& operator << (std::ostream& os, const Interval& tmp);
    
    unsigned start;
    unsigned end;
};

/*
 * the function in this namespace is specifically for the comparison using the node coloring of the regular expression of the walks
 */
namespace regex {

    /*
     * it takes in vector of node * as an input and according to the colors of the each nod  in regexGraph, it creates the virtual nodes and returns vector of regexNodes
     */
    std::vector<regexNode> pathAnalysis(std::vector<node *> path, regexGraph& rg, std::set<int> &baseColors, unsigned &baseIndex);

    /*
     * this function prints the vector of regexNode to the stdout
     */
    void pathPrint(std::vector<regexNode> &path);

    /*
     * this function is specifically for realigning after the divergence of the function
     * it will manipulate i and j to be at the convergence point
     * will return where it converged in terms of index
     * if it didn't converge, will return -1
     */
    unsigned regexRealignment(std::map<regexNode, std::vector<unsigned>> &realignmentMap, std::vector<regexNode> &path1, std::vector<regexNode> &path2, unsigned &i, unsigned &j, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, std::vector<std::tuple<unsigned, unsigned>> &alignedVirtualNodes);

    /*
     * this fills in the map for the realignment
     */
    void prepareMap(std::map<regexNode, std::vector<unsigned>> &realignmentMap, std::vector<regexNode> &path2, int j);

    /*
     * this is the function which adds all the parts that diverged and converged
     * I think I should add a dummy node if it didn't converge at the end of the iteration
     * (but you should delete it after you output them to avoid memory leaks)
     */
    void regexAlignment(std::vector<regexNode> &path1, std::vector<regexNode> &path2, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff, std::vector<Interval> &points);

    bool cmpInterval(const Interval &i1, const Interval &i2);
}
#endif /* REGEXNODE_H */
