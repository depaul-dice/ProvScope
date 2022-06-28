#ifndef REGEX_H
#define REGEX_H

#include "tools.h"
#include "cfg.h"
#include <set> 
#include <map>
#include <tuple>
#include <vector>
#include <iostream>
#include <fstream>

class regexGraph
{
public:
    // big 4
    regexGraph() = delete;
    regexGraph(const regexGraph&);
    regexGraph& operator = (const regexGraph&) = delete;
    ~regexGraph() = default;

    regexGraph(node *, std::set<node *>&, std::map<node *, std::set<node *>>&);
    bool isBackedge(node *src, node *dst);
    int getColorBackedge(node *src, node *dst);
    std::set<int> getColors(node *);
    void visualize(std::string filename);

    friend std::ostream& operator << (std::ostream& os, const regexGraph& tmp);

private:
    node *entryPoint;
    std::set<node *> nodes;
    std::map<node *, std::set<node *>> edges;
    std::map<node *, std::set<int>> colors; // this is used for grouping reasons
    std::map<node *, std::set<node *>> backedges; 
    std::map<node *, int> backedge2color;

    void writePrefix(std::ofstream &file);
    void writeEachNode(std::ofstream &file, node *tmp);
    void writeEachEdge(std::ofstream &file, node *src, node *dst);
    void colorNodes(node *curr, std::vector<node *>&, int &);
    void findGroupsRecursive(std::vector<node *> traversals, std::set<node *> history, std::set<node *> &done, node *curr, int &currColor);
    void findGroups(); // this colors all the nodes
    void printNodes();
    void printEdges();
};

#endif /* REGEX_H */
