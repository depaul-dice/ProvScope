#ifndef SUBGRAPH_H
#define SUBGRAPH_H

#include "tools.h"
#include "cfg.h"
#include <set> 
#include <map>
#include <tuple>
#include <vector>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

class subgraph
{
public:
    subgraph() = delete;
    subgraph(const subgraph&);
    subgraph& operator = (const subgraph&);
    ~subgraph();

    subgraph(std::vector<node *> path1, std::vector<node *> path2);
    std::map<node *, std::set<node *>> findPostDominators();
    //std::set<node *> getPostDominators(node *); // in case the node is virtual use the head

private:
    void dfs(node *Node, std::set<node *>& visited, std::vector<node *>& v);
    std::vector<node *> reverseTopSort();
    
    node *entryPoint;
    std::set<node *> nodes;
    std::map<node *, std::set<node *>> edges; // what do you do with the virtual edges?
    std::map<node *, std::set<node *>> backedges; // let's not fill this for now
    
public:
    std::map<node *, virtualNode *> virtualNodes;
};

#endif /* SUBGRAPH_H */
