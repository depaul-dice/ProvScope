#include "regexNode.h"

using namespace std;
using namespace Tools;

regexNode::regexNode(const regexNode& tmp)
    :isVirtual(tmp.isVirtual), colors(tmp.colors), content(tmp.content), originalIndex(tmp.originalIndex), nodes(tmp.nodes)
{
}

regexNode::regexNode(node *content, int index, std::set<int> colors)
    :isVirtual(false), colors(colors), content(content), originalIndex(index), nodes()
{
}

regexNode::regexNode(std::set<int> colors, vector<vector<regexNode>> nodes)
    :isVirtual(true), colors(colors), content(nullptr), originalIndex(-1), nodes(nodes)
{
}

node *regexNode::get()
{
    return content;
}

namespace regex
{
    
    // you need to do the check of the backedges here
    vector<regexNode> pathAnalysis(vector<node *>& path, regexGraph& rg, set<int> &baseColors, unsigned &baseIndex)
    {
        vector<regexNode> vec;
        node *currNode = nullptr;
        set<int> currColors(baseColors);
        unsigned currIndex = baseIndex;
        node *prevNode = nullptr;

        unsigned i = 0;
        while(i < path.size())
        { 
            currNode = path[i++];     
            currColors = rg.getColors(currNode); 
            if(!diff(currColors, baseColors).empty()) {
                /* 
                 * this means the color is added 
                 * 1. shove the nodes in the vector
                 * 2. call pathAnalysis recursively, notice the index has changed 
                 * 3. add the virtual node
                 */
                // this while loop should be within one iteration of loop
                // currColors are now base
                vector<vector<regexNode>> iterations;
                while(i < path.size() && isContained(currColors, rg.getColors(currNode)))
                {
                    vector<node *> tmpVec;
                    // while you haven't hit the back edge
                    while(!rg.isBackedge(prevNode, currNode))
                    {
                        tmpVec.push_back(currNode); 
                        prevNode = currNode;
                        currNode = path[i++];
                    }
                    vector<regexNode> oneIteration = pathAnalysis(tmpVec, rg, currColors, currIndex); // currIndex is updated here 
                    iterations.push_back(oneIteration);
                }
                regexNode rn(currColors, iterations);
                prevNode = nullptr;
            } else {
                /*
                 * this means the context was the same (thank god)
                 * initialize the regexNode and shove it in the vector
                 */
                regexNode rn(currNode, currIndex++, baseColors);
                vec.push_back(rn);
            }
        }
        baseIndex = currIndex; // updating the index for the caller
        return vec;
    }
}
