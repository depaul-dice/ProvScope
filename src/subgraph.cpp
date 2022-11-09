// question: how would you deal with the virtual nodes?
#include "subgraph.h"

using namespace std;
using namespace Tools;

subgraph::subgraph(const subgraph& tmp)
    :entryPoint(tmp.entryPoint), nodes(tmp.nodes), edges(tmp.edges), backedges(tmp.backedges), virtualNodes(tmp.virtualNodes)
{
}

subgraph& subgraph::operator = (const subgraph& tmp)
{
    entryPoint = tmp.entryPoint;
    nodes = tmp.nodes;
    edges = tmp.edges;
    backedges = tmp.backedges;
    virtualNodes = tmp.virtualNodes;
    return *this;
}

subgraph::~subgraph()
{
    for(auto it = virtualNodes.begin(); it != virtualNodes.end(); it++) {
        delete it->second;
    }
}

subgraph::subgraph(vector<node *>path1, vector<node *>path2) 
    :entryPoint(nullptr), nodes(), edges(), backedges(), virtualNodes()
{
    if(path1.size() > 0) {
        entryPoint = path1[0];
    } else {
        cerr << "path1 is not in the correct format\n";
        exit(1);
    }

    node *prev = nullptr;
    vector<node *> path = path1;
    virtualNode *vn = nullptr;
label:
    for(node *Node: path) {
        // in case of virtual node 
        if(Node->type == nodeType::Virtual) {
            // do you already have the virtual node?
            if(isContained(((virtualNode *)Node)->head, virtualNodes)) {
                vn = virtualNodes[((virtualNode *)Node)->head];
            } else { 
                vn = new virtualNode(*((virtualNode *)Node));
                virtualNodes[((virtualNode *)Node)->head] = vn;
            }
            Node = (node *)vn;
        }

        if(!isContained(Node, nodes)) {
            nodes.insert(Node);
        }

        if(prev != nullptr) {
            //cout << "adding " << *prev << "\n -> " << *Node << " as an edge\n";
            if(!isContained(prev, edges)) {
                edges[prev];
            }
            edges[prev].insert(Node);
        }
        prev = Node;
    } 

    if(path != path2) {
        //cout << "now at path 2\n";
        path = path2;
        prev = nullptr;
        goto label;
    }

    //cout << "These are the nodes\n" << nodes << endl;
    //cout << "These are the edges\n" << edges << endl;
}

map<node *, set<node *>> subgraph::findPostDominators() 
{
    map<node *, set<node *>> pd{};
    /*
    map<node *, set<node *>> rEdges{}; // I will get the edges reversed
    node *src;
    for(auto it = edges.begin(); it != edges.end(); it++) {
        src = it->first;
        cout << "src: " << *src << endl;
        for(auto dst: it->second) {
            if(!isContained(dst, rEdges)) {
                rEdges[dst];
            }
            cout << "dst: " << *dst << endl;
            rEdges[dst].insert(src);
        }
    }
    */
    //cout << "at find postDominators\n";
    vector<node *> v = reverseTopSort();
    /*
    cout << "i have done top sort: " << v.size() << endl;
    for(auto e: v) {
        cout << *e << endl;
    }
    */
    
    bool alpha;
    for(auto src: v) {
        alpha = true;
        //cout << "src: " << *src << endl;
        for(auto dst: edges[src]) {
            //cout << *dst << endl;
            if(dst == src) continue;
            if(alpha) {
                pd[src] = pd[dst];
                alpha = false;
            } else {
                pd[src] = intersection(pd[src], pd[dst]);
            }
        }
        pd[src].insert(src);
    }

    //cout << pd << endl;
    return pd;
}

/*
set<node *> subgraph::getPostDominators(node *) 
{
}
*/

void subgraph::dfs(node *Node, set<node *>& visited, vector<node *>& v) 
{
    for(auto d: edges[Node]) {
       if(!isContained(d, visited)) {
           visited.insert(d);
           dfs(d, visited, v); 
       }
    }  

    //cout << "inserting " << *Node << endl;
    v.push_back(Node);
    return;
}

// topsort the whole graph
vector<node *> subgraph::reverseTopSort() 
{
    set<node *> visited{};  
    vector<node *> v{};

    for(auto Node: nodes) {
        //cout << "at " << *Node << endl;
        if(!isContained(Node, visited)) {
            visited.insert(Node);
            dfs(Node, visited, v);
        }
    } 

    return v;
}
