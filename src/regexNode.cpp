#include "regexNode.h"

using namespace std;
using namespace Tools;

regexNode::regexNode(const regexNode& tmp)
    :_isVirtual(tmp._isVirtual), colors(tmp.colors), content(tmp.content), originalIndex(tmp.originalIndex), nodes(tmp.nodes)
{
}

regexNode::regexNode(node *content, int index, std::set<int> colors)
    :_isVirtual(false), colors(colors), content(content), originalIndex(index), nodes()
{
}

regexNode::regexNode(std::set<int> colors, vector<vector<regexNode>> nodes)
    :_isVirtual(true), colors(colors), content(nullptr), originalIndex(-1), nodes(nodes)
{
}

bool regexNode::operator == (const regexNode& rhs) const
{
    return _isVirtual == rhs._isVirtual && colors == rhs.colors && content == rhs.content && originalIndex == rhs.originalIndex && nodes == rhs.nodes; 
}

bool regexNode::operator != (const regexNode &rhs) const
{
    return !(*this == rhs);
}

bool regexNode::operator < (const regexNode& rhs) const
{
    if(*this == rhs) return false;

    if(content != nullptr)
    {
        return content < rhs.content;
    }
    else
    {
        if(colors != rhs.colors) return colors < rhs.colors;
        if(originalIndex != rhs.originalIndex) return originalIndex < rhs.originalIndex;
        if(nodes.size() != rhs.nodes.size()) return nodes.size() < rhs.nodes.size();
        for(unsigned i = 0; i < nodes.size(); i++)
        {
            if(nodes[i].size() != rhs.nodes[i].size()) return nodes[i].size() != rhs.nodes.size();
        }

        for(unsigned i = 0; i < nodes.size(); i++)
        {
            for(unsigned j = 0; j < nodes[i].size(); j++)
            { 
                if(nodes[i][j] != rhs.nodes[i][j])
                {
                    return nodes[i][j] < rhs.nodes[i][j];
                } 
            }
        }
        return false;
    }
}

unsigned regexNode::getIndex()
{
    return originalIndex;
}

bool regexNode::isVirtual()
{
    return _isVirtual;
}

node *regexNode::get()
{
    return content;
}

std::vector<std::vector<regexNode>> regexNode::getInside()
{
    return nodes;
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

    void prepareMap(map<regexNode, vector<unsigned>> &realignmentMap, vector<regexNode> &path2, unsigned j)
    {
        for(unsigned index = j; index < path2.size(); index++)
        {
            realignmentMap[path2[index]].push_back(index);
        }
        return;
    }

    void regexRealignment(map<regexNode, vector<unsigned>> &realignmentMap, vector<regexNode> &path1, vector<regexNode> &path2, unsigned &i, unsigned &j, vector<tuple<unsigned, unsigned>> &alignedPairs, vector<tuple<unsigned, unsigned>> &alignedVirtualNodes, vector<unsigned> &convergencePoints)
    {
        unsigned size;
        if(realignmentMap.size() == 0) 
            prepareMap(realignmentMap, path2, j); 
        unsigned j_start = j;
        for(unsigned index = i; index < path1.size(); index++)
        {
            if(isContained(path2[index], realignmentMap)) 
            {
                vector<unsigned> &currVec = realignmentMap[path2[index]]; 
                size = currVec.size();
                for(unsigned t = 0; t < size; t++)
                {
                    if(currVec[t] >= j_start)
                    {
                        i = index; // change the i into index
                        j = currVec[t];
                        if(path2[index].isVirtual()) 
                        {
                            // aligned with the virtual node at i and j
                            alignedVirtualNodes.push_back(make_tuple(i, j));
                        }
                        else
                        {
                            // aligned at normal nodes
                            alignedPairs.push_back(make_tuple(path1[i].getIndex(), path2[j].getIndex()));
                        }
                        convergencePoints.push_back(i);
                        for(unsigned alpha = 0; alpha < t; alpha++)
                        {
                            currVec.erase(currVec.begin());
                        }
                        break;
                    }
                }
            }
            // else just go to the next alignment
        } 
    }

    void regexAlignment(vector<regexNode> &path1, vector<regexNode> &path2, vector<tuple<unsigned, unsigned>> &alignedPairs, int &diff)
    {
        unsigned i = 0, j = 0;
        vector<unsigned> divergencePoints; // necessary to sort this later and push them out
        vector<unsigned> convergencePoints; // necessary to sort this and push them out
        vector<tuple<unsigned, unsigned>> alignedVirtualNodes;
        map<regexNode, vector<unsigned>> realignmentMap;

        while(i < path1.size() && j < path2.size())
        {
            if(path1[i] == path2[j]) // I might need to implement operator for this
            {
                // if path1[i] is NOT virtual, put them in aligned pairs
                // else put them in alignedVirtualNodes
                if(path1[i].isVirtual()) 
                {
                    alignedVirtualNodes.push_back(make_tuple(i, j));
                }
                else
                {
                    alignedPairs.push_back(make_tuple(path1[i].getIndex(), path2[j].getIndex()));
                }
                
                i++; j++;
            }
            else
            {
                divergencePoints.push_back(i - 1);
                // you need to do the realignment here! 
                diff++;
                regexRealignment(realignmentMap, path1, path2, i, j, alignedPairs, alignedVirtualNodes, convergencePoints);
            }
        }

        
        vector<vector<regexNode>> content1, content2;
        for(unsigned index = 0; index < alignedVirtualNodes.size(); index++)
        {
            i = get<0>(alignedVirtualNodes[index]);
            j = get<1>(alignedVirtualNodes[index]);
            content1 = path1[i].getInside(); 
            content2 = path2[j].getInside();
            if(content1.size() != content2.size()) 
            {
                cout << "the size of the iteration is different between content1: " << content1.size() << " and content2: " << content2.size() << endl;
                diff++;
            }  
            unsigned iterations = min(content1.size(), content2.size());
            for(unsigned t = 0; t < iterations; t++)
            {
                regexAlignment(content1[t], content2[t], alignedPairs, diff);
            }
        }
    }
}
