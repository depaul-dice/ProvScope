#ifndef REGEX_H
#define REGEX_H

#include "cfg.h"
#include <unordered_set>
#include <unordered_graph> 
#include <vector>
#include <tuple>

class regexGraph
{
public:
    // big 4
    regexGraph() = delete;
    regexGraph(const regexGraph&) = delete;
    regexGraph& operator = (const regexGraph&) = delete;
    ~regexGraph() = default;

    regexGraph(std::unordered_set<node *>, std::unordered_map<node *, std::unordered_set<node *>>);
    bool isBackedge(node *src, node *dst);

private:
    node *entryPoint;
    std::unordered_set<node *> &nodes;
    std::unordered_map<node *, std::unordered_set<node *>> &edges;
    std::unordered_map<node *, std::unordered_set<int>> colors; // this is used for grouping reasons
    std::unordered_map<node *, std::unordered_set<node *>> backedges; // this should be the subset of edges
    std::unordered_map<std::tuple<node *, node *> edge, int color> backedge2color;
 
    void colorNodes(node *curr, std::vector<node *>& traversals);
    void findGroupsRecursive(std::vector<node *> traversals, std::unordered_set<node *> history, std::unordered_set<node *> &done, node *curr, int &currColor);
    void findGroups(); // this colors all the nodes
}

#endif /* REGEX_H */
