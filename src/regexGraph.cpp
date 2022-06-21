
#include "regexGraph.h"
using namespace std;
using namespace Tools;

regexGraph::regexGraph(const regexGraph& regex)
    :entryPoint(regex.entryPoint), nodes(regex.nodes), edges(regex.edges), colors(regex.colors), backedges(regex.backedges), backedge2color(regex.backedge2color)
{
}

regexGraph::regexGraph(node *ep, set<node *> &nodes, map<node *, set<node *>> &edges) : entryPoint(ep), nodes(nodes), edges(edges)
{
    findGroups(); 
}


/*
 * the 2 static function below are to simply save my typing of doing the find and checking if it is the end or not
 */
/*
template <typename K, typename V>
static inline bool isContained(K key, map<K, V> table)
{
    auto it = table.find(key);
    if(it != table.end())
        return true;
    else 
        return false;
}

template <typename K>
static inline bool isContained(K key, set<K> table)
{
    auto it = table.find(key);
    if(it != table.end())
        return true;
    else
        return false;
}
*/

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
    while(traversals[index] != curr)
    {
        tmp = traversals[index]; 
        if(isContained(tmp, colors)) 
        {
            set<int> set = colors[tmp];
            set.insert(currColor);
            colors[tmp] = set;
        }
        else
        {
            set<int> set{currColor};
            colors[tmp] = set;
        }
        index--;
    }
    currColor++;
}

/*
 * this function recursively calls itself and traverse through the nodes to find backedges in the graph
 */
void regexGraph::findGroupsRecursive(vector<node *> traversals, set<node *> history, set<node *> &done, node *curr, int &currColor)
{
    if(isContained(curr, done)) return; // we have already visited this node
    // else
    if(isContained(curr, history))
    {
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
    if(isContained(src, backedges)) // is any backedge coming out of the source?
    {
        if(isContained(dst, backedges[src])) // is the edge to dst from src backedge?
            return true;
    }
    return false;
}
