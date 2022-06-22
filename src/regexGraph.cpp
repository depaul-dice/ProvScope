
#include "regexGraph.h"
using namespace std;
using namespace Tools;

regexGraph::regexGraph(const regexGraph& regex)
    :entryPoint(regex.entryPoint), nodes(regex.nodes), edges(regex.edges), colors(regex.colors), backedges(regex.backedges), backedge2color(regex.backedge2color)
{
}

regexGraph::regexGraph(node *ep, set<node *> &nodes, map<node *, set<node *>> &edges) : entryPoint(ep), nodes(nodes), edges(edges)
{
    printNodes();
    printEdges();
    findGroups(); 
}


/*
 * this function adds the backedges and colors
 * backedge is added for the back of the traversal and colors are added until it goes back to curr
 */
void regexGraph::colorNodes(node *curr, vector<node *>& traversals, int &currColor)
{
    // let's first add the backedge
    node *prev = traversals.back();
    if(isContained(prev, backedges))
    {
        set<node *> dsts = backedges[prev];
        dsts.insert(curr);
        backedges[prev] = dsts;
    }
    else
    {
        set<node *> dsts{curr};
        backedges[prev] = dsts;
    }
    tuple<node *, node *> edge(make_tuple(prev, curr));  
    backedge2color[edge] = currColor;

    // then let's add the color for all the nodes
    unsigned index = traversals.size() - 1;
    
    node *tmp;
    do
    {
        tmp = traversals[index]; 
        cout << "coloring " <<  *tmp << endl;
        if(isContained(tmp, colors)) 
        {
            colors[tmp].insert(currColor);
        }
        else
        {
            set<int> set{currColor};
            colors[tmp] = set;
        }
    } while(traversals[index--] != curr);
    currColor++;
}

/*
 * this function recursively calls itself and traverse through the nodes to find backedges in the graph
 */
void regexGraph::findGroupsRecursive(vector<node *> traversals, set<node *> history, set<node *> &done, node *curr, int &currColor)
{
    cout << "at " << *curr << endl;
    if(isContained(curr, done)) {
        cout << "have already visited. returning\n";
        return; // we have already visited this node
    }
    // else
    if(isContained(curr, history))
    {
        cout << "contained in the history, coloring nodes\n";
        colorNodes(curr, traversals, currColor);
    }
    else
    {
        traversals.push_back(curr);
        history.insert(curr);

        set<node *>& dsts{edges[curr]};
        for(auto it = dsts.begin(); it != dsts.end(); it++)
        {
            findGroupsRecursive(traversals, history, done, *it, currColor);
        }
        done.insert(curr);
    }
    return;
}

/*
 * this function is basically doing the dfs with nodes that have been visited in the vector
 */
void regexGraph::findGroups() 
{
    node *curr = entryPoint;
    vector<node *> traversals{curr};
    set<node *> history{curr};
    set<node *> done{curr};
    int currColor = 0;
    cout << "at " << *curr << endl;

    // go through all the destinations available
    set<node *>& dsts{edges[curr]};
    for(auto it = dsts.begin(); it != dsts.end(); it++)
    {
        findGroupsRecursive(traversals, history, done, *it, currColor);
    }
    done.insert(curr);
    return;
}

/*
 * if the src is contained in backedges and dst is contained in backedges[src], return true
 * otherwise return false
 */
bool regexGraph::isBackedge(node *src, node *dst)
{
    if(isContained(src, backedges)) // is any backedge coming out of the source? the function is from tools.h
    {
        if(isContained(dst, backedges[src])) // is the edge to dst from src backedge?
            return true;
    }
    return false;
}

void regexGraph::printNodes()
{
    std::cout << "these are nodes\n";
    print_set(nodes);
}

void regexGraph::printEdges()
{
    std::cout << "\nthose are edges\n";
    print_map(edges);
}
