#ifndef REGEX_H
#define REGEX_H

#include "tools.h"
#include "cfg.h"
#include <set> 
#include <map>
#include <tuple>
#include <vector>

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

private:
    node *entryPoint;
    std::set<node *> nodes;
    std::map<node *, std::set<node *>> edges;
    std::map<node *, std::set<int>> colors; // this is used for grouping reasons
    std::map<node *, std::set<node *>> backedges; // this should be the subset of edges
    std::map<std::tuple<node *, node *>, int> backedge2color;

    void colorNodes(node *curr, std::vector<node *>&, int &);
    void findGroupsRecursive(std::vector<node *> traversals, std::set<node *> history, std::set<node *> &done, node *curr, int &currColor);
    void findGroups(); // this colors all the nodes
    void printNodes();
    void printEdges();
};

#endif /* REGEX_H */
