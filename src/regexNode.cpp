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

ostream& operator << (ostream& os, const regexNode& tmp)
{
    if(tmp.content != nullptr) {
        os << "non-virtual: " << *tmp.content << ", " << tmp.originalIndex << ", " << tmp.colors;
    } else {
        os << "virtual: " << tmp.colors << endl;
        for(unsigned i = 0; i < tmp.nodes.size(); i++)
        {
            os << "\tin " << i << "th loop\n";
            for(unsigned j = 0; j < tmp.nodes[i].size(); j++)
            {
                os << "\t" << tmp.nodes[i][j] << endl;
            }
        }
    }
    return os;
}

namespace regex
{
    
    // you need to do the check of the backedges here
    vector<regexNode> pathAnalysis(vector<node *> path, regexGraph& rg, set<int> &baseColors, unsigned &baseIndex)
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
            set<int> diffColors = diff(currColors, baseColors);

            assert(diffColors.size() < 2); // this case will be taken care of later
            if(!diffColors.empty()) {
                /* 
                 * this means the color is added 
                 * 1. shove the nodes in the vector
                 * 2. call pathAnalysis recursively, notice the index has changed 
                 * 3. add the virtual node
                 */
                // this while loop should be within one iteration of loop
                // currColors are now base
                // cout << "diffColors were not empty\n";
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
                        if(i >= path.size() || !isContained(currColors, rg.getColors(currNode)))
                            break;
                    }

                    if(rg.isBackedge(prevNode, currNode)) 
                    {
                        prevNode = nullptr;
                    }
                    /*
                    cout << "printing out tmpVec before doing the recursive calls\n";
                    for(unsigned j = 0; j < tmpVec.size(); j++)
                    {
                        cout << *tmpVec[j] << endl;
                    }
                    */

                    vector<regexNode> oneIteration = pathAnalysis(tmpVec, rg, currColors, currIndex); // currIndex is updated here 
                    iterations.push_back(oneIteration);
                }
                regexNode rn(currColors, iterations);
                vec.push_back(rn);
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

    void pathPrint(vector<regexNode> &path) {
        cout << "in pathPrint\n";
        for(unsigned i = 0; i < path.size(); i++)
        {
            cout << path[i] << endl;
        }
    }

    void regexAlignment() {
    }
}
