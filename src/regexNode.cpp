// for the code that is comparig the interval, I have referred to https://www.geeksforgeeks.org/sorting-a-vector-in-c/

#include "regexNode.h"

using namespace std;
using namespace Tools;

regexNode::regexNode(const regexNode& tmp)
    :_isVirtual(tmp._isVirtual), colors(tmp.colors), content(tmp.content), originalIndex(tmp.originalIndex), nodes(tmp.nodes)
{
}

/*
 * constructor for non-virtual nodes
 */
regexNode::regexNode(node *content, int index, std::set<int> colors)
    :_isVirtual(false), colors(colors), content(content), originalIndex(index), nodes()
{
}

/*
 * constructor for virtua nodes
 */
regexNode::regexNode(std::set<int> colors, node *startNode, vector<vector<regexNode>> nodes)
    :_isVirtual(true), colors(colors), content(startNode), originalIndex(-1), nodes(nodes)
{
}

/*
 * necessary for the map implementation
 */
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

void regexNode::setIndex(unsigned index)
{
    originalIndex = index;
}

/*
 * for printing nodes
 */
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

Interval::Interval(unsigned start, unsigned end)
    :start(start), end(end)
{
}

ostream& operator << (ostream& os, const Interval& tmp)
{
    os << "{" << tmp.start << ", " << tmp.end << "}";
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
AfterLoop:
            currColors = rg.getColors(currNode); 
            set<int> diffColors = currColors - baseColors;

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
                cout << "diffColors were not empty\nNow it is :" << currColors;
                
                vector<vector<regexNode>> iterations;
                node *startNode = currNode;
                while(i < path.size() && isContained(currColors, rg.getColors(currNode)))
                {
                    vector<node *> tmpVec;
                    // while you haven't hit the back edge
                    while(!rg.isBackedge(prevNode, currNode))
                    {
                        tmpVec.push_back(currNode); 
                        prevNode = currNode;
                        currNode = path[i++];
                        if(i >= path.size() || !isContained(currColors, rg.getColors(path[i])))
                            break;
                    }

                    if(rg.isBackedge(prevNode, currNode)) 
                    {
                        prevNode = nullptr;
                    }
                    cout << "printing out tmpVec before doing the recursive calls\n";
                    for(unsigned j = 0; j < tmpVec.size(); j++)
                    {
                        cout << *tmpVec[j] << endl;
                    }

                    vector<regexNode> oneIteration = pathAnalysis(tmpVec, rg, currColors, currIndex); // currIndex is updated here 
                    iterations.push_back(oneIteration);
                }
                regexNode rn(currColors, startNode, iterations);
                rn.setIndex(currIndex - 1);
                vec.push_back(rn);
                prevNode = nullptr;
                goto AfterLoop;
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

    /*
     * printing out the path here
     */
    void pathPrint(vector<regexNode> &path) {
        cout << "in pathPrint\n";
        for(unsigned i = 0; i < path.size(); i++)
        {
            cout << path[i] << endl;
        }
    }

    /*
     * filling in the map for the realignment
     */
    void prepareMap(map<regexNode, vector<unsigned>> &realignmentMap, vector<regexNode> &path2, unsigned j)
    {
        for(unsigned index = j; index < path2.size(); index++)
        {
            realignmentMap[path2[index]].push_back(index);
        }
        return;
    }

    /* 
     * this function is used when the divergence happened
     */
    unsigned regexRealignment(map<regexNode, vector<unsigned>> &realignmentMap, vector<regexNode> &path1, vector<regexNode> &path2, unsigned &i, unsigned &j, vector<tuple<unsigned, unsigned>> &alignedPairs, vector<tuple<unsigned, unsigned>> &alignedVirtualNodes)
    {
        unsigned size;
        unsigned convergencePoint = (unsigned)-1;
        if(realignmentMap.size() == 0) 
            prepareMap(realignmentMap, path2, j); 
        unsigned j_start = j;
        for(unsigned index = i; index < path1.size(); index++)
        {
            if(isContained(path1[index], realignmentMap)) 
            {
                vector<unsigned> &currVec = realignmentMap[path1[index]]; 
                size = currVec.size();
                for(unsigned t = 0; t < size; t++)
                {
                    if(currVec[t] >= j_start)
                    {
                        i = index; // change the i into index
                        j = currVec[t];
                        cout << "realigned at " << path1[i] << endl;
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

                        convergencePoint = path1[i].getIndex();
                        for(unsigned alpha = 0; alpha < t; alpha++)
                        {
                            currVec.erase(currVec.begin());
                        }
                        return convergencePoint;
                    }
                }
            }
            // else just go to the next alignment
        } 
        return convergencePoint;
    }

    /*
     * this function is the API of the comparison
     */
    void regexAlignment(vector<regexNode> &path1, vector<regexNode> &path2, vector<tuple<unsigned, unsigned>> &alignedPairs, int &diff, vector<Interval> &points)
    {
        unsigned i = 0, j = 0;
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
                    cout << "aligned at virtual node: " << path1[i] << endl;
                    alignedVirtualNodes.push_back(make_tuple(i, j));
                }
                else
                {
                    cout << "aligned at non-virtual node: " << path1[i] << endl;
                    alignedPairs.push_back(make_tuple(path1[i].getIndex(), path2[j].getIndex()));
                }
                
                i++; j++;
            }
            else
            {
                unsigned start;
                unsigned end;
                if(i > 0) {
                    start = path1[i - 1].getIndex();
                    //cout << "this is the index of divergence: " << start << endl;
                } else {
                    start = (unsigned)-1;
                }
                // you need to do the realignment here! 
                cout << "did not align at " << path1[i] << " vs. " << path2[j] << endl;
                using namespace Tools;
                diff++;
                end = regexRealignment(realignmentMap, path1, path2, i, j, alignedPairs, alignedVirtualNodes);
                points.push_back(Interval(start, end));
                i++; j++;
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
            // doing the regexAlignment recursively for the virtual nodes
            for(unsigned t = 0; t < iterations; t++)
            {
                regexAlignment(content1[t], content2[t], alignedPairs, diff, points);
            }
        }

    }

    bool cmpInterval(const Interval &i1, const Interval &i2)
    {
        return i1.start < i2.start;
    }
}
