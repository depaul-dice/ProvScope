
#include "Comparison.h"
using namespace Tools;

/*
Comparison::Comparison(std::vector<node *>& path1, std::vector<node *>& path2)
    :hpath(path1), vpath(path2), loopPath1(std::vector()), loopPath2(), matrix(path2.size() + 1, path1.size() + 1)
{
}
*/

Comparison::Comparison(std::vector<node *>& path1, std::vector<node *>& path2, std::vector<std::tuple<node *, unsigned>>& lp1, std::vector<std::tuple<node *, unsigned>>& lp2)
    :hpath(path1), vpath(path2), loopPath1(lp1), loopPath2(lp2), matrix(path2.size() + 1, path1.size() + 1)
{
} 

void Comparison::printMatrix(std::ostream &os)
{
    os << matrix;
}

void Comparison::editDistanceInitSetUp(void)
{
    // matrix.setElement(0, 0, 0) <- already done with calloc
    
    float val = 0;
    for(unsigned j = 1; j < matrix.getWidth(); j++)
    {
        val += insertCost;
        matrix.setElement(0, j, val);        
    }
    
    val = 0;
    for(unsigned i = 1; i < matrix.getLength(); i++)
    {
        val += insertCost;
        matrix.setElement(i, 0, val);
    }
}

float Comparison::smallest(float a, float b, float c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

ErrorCode Comparison::fillMatrix(void)
{
    editDistanceInitSetUp();
    float val;
    for(unsigned i = 1; i < matrix.getLength(); i++)
    {
        for(unsigned j = 1; j < matrix.getWidth(); j++)
        {
            if(hpath[j - 1] == vpath[i - 1])
            {
                val = smallest(matrix.getElement(i - 1, j - 1), matrix.getElement(i, j - 1), matrix.getElement(i - 1, j));
                matrix.setElement(i, j, val);
            }
            else
            {
                val = smallest(matrix.getElement(i - 1, j - 1) +  alternateCost, matrix.getElement(i, j - 1) + insertCost, matrix.getElement(i - 1, j) + insertCost);
                matrix.setElement(i, j, val);
            }
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode Comparison::returnEC(unsigned i, unsigned j)
{
    assert(i == 0 || j == 0);

    if(i == 0 && j == 0)
        return ErrorCode::SUCCESS;
    else 
    {
        fprintf(stderr, "finding trace for divergence/convergence failed\n");
        fprintf(stderr, "failed in terms of indexing\n");
        return ErrorCode::INVALID_INDEX;
    }
}

ErrorCode Comparison::findMinCostTrace(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs)
{
    float val;

    unsigned i = matrix.getLength() - 1;
    unsigned j = matrix.getWidth() - 1; 
    unsigned prevI = (unsigned)-1;
    unsigned prevJ = (unsigned)-1;

    while(true)
    {
        // val is instantiated above
        val = matrix.getElement(i, j);
        if(hpath[j - 1] == vpath[i - 1])
        {
            /*
            hpath[j - 1]->printNodeSimply(std::cout);
            std::cout << "\nj - 1: " << j - 1 << " vs. i - 1: " << i - 1 << std::endl;
            */
            if(prevI != i && prevJ != j)
            {
                alignedPairs.insert(alignedPairs.begin(), std::make_tuple(j - 1, i - 1));
                prevI = i;
                prevJ = j;
            }

            if(val == matrix.getElement(i - 1, j - 1))
            {
                i--; j--;
                if(i == 0 || j == 0) 
                    return returnEC(i, j); 
            }
            else if(val == matrix.getElement(i - 1, j))
            {
                i--;
                if(i == 0 || j == 0) 
                    return returnEC(i, j); 
            }
            else if(val == matrix.getElement(i, j - 1))
            {
                j--;
                if(i == 0 || j == 0) 
                    return returnEC(i, j); 
            }
            else
            {
                fprintf(stderr, "finding trace for divergence/convergence failed\nCould not find the right value at (i, j) == (%d, %d)\n", i, j);
                return ErrorCode::INVALID_VALUE;
            }
        } 
        else
        {
            if(val - alternateCost == matrix.getElement(i - 1, j - 1))
            {
                i--; j--;
                if(i == 0 || j == 0) 
                    return returnEC(i, j); 
            }
            else if(val - insertCost == matrix.getElement(i - 1, j ))
            {
                i--;
                if(i == 0 || j == 0) 
                    return returnEC(i, j); 
            }
            else if(val - insertCost == matrix.getElement(i, j - 1))
            {
                j--;
                if(i == 0 || j == 0) 
                    return returnEC(i, j); 
            }
            else
            {
                // this is an error
                fprintf(stderr, "finding trace for divergence/convergence failed\nCould not find the right value at (i, j) == (%d, %d)\n", i, j);
                return ErrorCode::INVALID_VALUE;
            }
        }
    }

}

ErrorCode Comparison::editDistance(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs)
{
    ErrorCode ec;
    ec = fillMatrix();
    if(ec != ErrorCode::SUCCESS)
        return ec;
    ec = findMinCostTrace(alignedPairs);
    return ec;
}

void Comparison::greedyApproach(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    std::vector<node *>& path1 = hpath;
    std::vector<node *>& path2 = vpath;

    unsigned i = 0; unsigned j = 0;
    std::map<node *, std::vector<unsigned>> match;
    unsigned j_start;

    while(i < path1.size() && j < path2.size())
    {
        if(path1[i] == path2[j])
        {
            alignedPairs.push_back(std::make_tuple(i, j));
            //std::cout << "adding (top) :" << i << ", " << j << std::endl;
            i++; j++;
        }
        else
        {
            diff++;
            std::cout << "diverted at ";
            path1[i - 1]->printNodeSimply(std::cout);
            std::cout << " in i: " << i - 1 << " and j: " << j - 1 << std::endl;
            // preparing the hashmap
            if(match.size() == 0)
            {
                for(unsigned k = j; k < path2.size(); k++)
                {
                    //std::cout << "adding :" << *path2[k] <<std::endl;
                    match[path2[k]].push_back(k);
                } 
            }

            j_start = j;
            // search for each node in path 1 if it exists in path2
            while(i < path1.size())
            {
                if(match.find(path1[i]) == match.end())
                {
                    //std::cout << "did not find for " << *(path1[i]) << "i: " << i << std::endl;
                    i++;
                    //std::cout << "i: " << i << std::endl;
                }
                else
                {
                    //std::cout << "found for " << *(path1[i]) << "i: " << i << std::endl;
                    std::vector<unsigned>& vec = match[path1[i]];
                    bool found = false;
                    unsigned size = vec.size();
                    //std::cout << vec[0] << ',' << j_start << std::endl;
                    for(unsigned t = 0; t < size; t++)
                    {
                        if(vec[t] >= j_start)
                        {
                            j = vec[t];
                            found = true;
                            //std::cout << "adding " << i << "," << j << std::endl;
                            alignedPairs.push_back(std::make_tuple(i, j));
                            for(unsigned alpha = 0; alpha < t; alpha++)
                            {
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found)
                    {
                        i++;
                    }
                    else
                    {
                        std::cout << "converged at ";
                        path1[i]->printNodeSimply(std::cout);
                        std::cout << " in i: " << i << " and j: " << j << std::endl;
                        i++; j++;
                        break;
                    }
                }
            }
        } 
    }
}

bool Comparison::nodeMatch(node *a, node *b)
{
    if(a->type == nodeType::Virtual && b->type == nodeType::Virtual)
    {
        if(strcmp(a->name, b->name) == 0) return true;
        else return false;
    }
    else
    {
        if(a == b) return true;
        else return false;
    }
}

ErrorCode Comparison::__loopGreedyContents(std::vector<std::tuple<node *,unsigned>>& content1, std::vector<std::tuple<node *, unsigned>>& content2, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    unsigned i = 0; unsigned j = 0;
    std::map<node *, std::vector<unsigned>> match;
    std::map<std::string, std::vector<unsigned>> virtualNodeMatch;

    node *nodeOne;
    node *nodeTwo;
    unsigned indexOne;
    unsigned indexTwo;
    unsigned j_start;
    bool aligned = true;

    while(i < content1.size() && j < content2.size())
    {
        nodeOne = std::get<0>(content1[i]);
        nodeTwo = std::get<0>(content2[j]);
        indexOne = std::get<1>(content1[i]);
        indexTwo = std::get<1>(content2[j]);
        if(this->nodeMatch(nodeOne, nodeTwo))
        {
            if(nodeOne->type != nodeType::Virtual)
            {
                alignedPairs.push_back(std::make_tuple(indexOne, indexTwo));
                //std::cout << "pushing i: " << indexOne << ", j: " << indexTwo << ", size: " << alignedPairs.size() << std::endl;
            }
            else
            {
                if(this->__loopGreedyContents(((virtualNode *)nodeOne)->contents, ((virtualNode *)nodeTwo)->contents, alignedPairs, diff) != ErrorCode::SUCCESS)
                {
                    std::cerr << "Failed in virtual node content\n";
                    exit(1);
                }
            }
            /*
            std::cout << "node matched: ";
            nodeOne->printNodeSimply(std::cout); 
            std::cout << '\n';
            */
            i++; j++;
        }
        else
        {
            aligned = false;
            diff++;
            std::cout << "diverted at ";
            (std::get<0>(content1[i - 1]))->printNodeSimply(std::cout);
            //nodeOne->printNodeSimply(std::cout);
            std::cout << " in i: " << i - 1 << " and j: " << j - 1 << std::endl;

        
            // if the matching hashmap is not filled, fill it up
            if(match.size() == 0 && virtualNodeMatch.size() == 0)
            {
                node *tmpNode;
                for(unsigned k = j; k < content2.size(); k++)
                {
                    tmpNode = std::get<0>(content2[k]);
                    if(tmpNode->type == nodeType::Virtual)
                    {
                        virtualNodeMatch[tmpNode->name].push_back(k);
                    }
                    else
                    {
                        match[tmpNode].push_back(k);
                    }
                }
            }

            j_start = j;
            while(i < content1.size())
            {
                node *node2Match = std::get<0>(content1[i]);
                unsigned indexOne = std::get<1>(content1[i]);
                unsigned indexTwo;
                node *nodeMatched;
                //unsigned indexTwo;
                if(node2Match->type != nodeType::Virtual && match.find(node2Match) != match.end())
                {
                    // this is when it was found in the normal one
                    std::vector<unsigned> &vec = match[node2Match];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++)
                    {
                        if(vec[t] >= j_start)
                        {
                            j = vec[t];
                            nodeMatched = std::get<0>(content2[j]);
                            indexTwo = std::get<1>(content2[j]);
                            //std::cout << "node found: ";
                            //nodeMatched->printNodeSimply(std::cout);
                            
                            //std::cout << "\t" << indexTwo << std::endl;
                            found = true;
                            for(unsigned alpha = 0; alpha < t; alpha++)
                            { 
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found)
                    {
                        std::cout << "NODE NOT FOUND\n";
                        i++;
                    }
                    else
                    {
                        std::cout << "converged at ";
                        node2Match->printNodeSimply(std::cout);
                        std::cout << " in i: " << indexOne << " and j: " << indexTwo << std::endl;
                        alignedPairs.push_back(std::make_tuple(indexOne, indexTwo));
                        //std::cout << "pushing i: " << indexOne << ", j: " << indexTwo << ", size: " << alignedPairs.size() << std::endl;
                        aligned = true;
                        i++; j++;
                        break;
                    }
                }
                else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) == virtualNodeMatch.end())
                {
                    // this is when it was found in the virtual node
                    std::vector<unsigned> &vec = virtualNodeMatch[node2Match->name];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++)
                    {
                        if(vec[t] >= j_start)
                        {
                            j = vec[t];
                            found = true;
                            //alignedVN.push_back(std::make_tuple(i, j));
                            for(unsigned alpha = 0; alpha < t; alpha++)
                            {
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found)
                    {
                        i++;
                    }
                    else
                    {
                        std::cout << "converged at ";
                        node2Match->printNodeSimply(std::cout);
                        std::cout << " in i: " << indexOne << " and j: " << indexTwo << std::endl; 
                        i++; j++;
                        aligned = true;
                        if(this->__loopGreedyContents(((virtualNode *)node2Match)->contents, ((virtualNode *)nodeTwo)->contents, alignedPairs, diff) != ErrorCode::SUCCESS)
                        {
                            std::cerr << "something wrong with the __loopGreedyContents\n";
                            exit(1);
                        }
                        break;
                    }
                }
                else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) != virtualNodeMatch.end())
                {
                    // this is when it was not found as the virtual node
                    i++;
                }
                else
                {
                    // this is when it was not found as not a virtual node
                    i++;
                }
            }


            if(aligned == false)
            {
                node *tmpNode = std::get<0>(content1[0]);
                std::cout << "converged at ";
                tmpNode->printNodeSimply(std::cout);
                std::cout << "at the end\n";
            }

        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode Comparison::loopGreedyApproach(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    /*
    std::vector<node *>& path1 = hpath;
    std::vector<node *>& path2 = vpath;
    */

    /* 
     * step 1. get the matching pairs of the nodes 
     */

    unsigned i = 0; unsigned j = 0; // these only consider the match at the surface
    std::map<node *, std::vector<unsigned>> match;
    std::map<std::string, std::vector<unsigned>> virtualNodeMatch;

    node *nodeOne;
    node *nodeTwo;
    unsigned indexOne;
    unsigned indexTwo;
    unsigned j_start;

    while(i < loopPath1.size() && j < loopPath2.size())
    {
        nodeOne = std::get<0>(loopPath1[i]); 
        nodeTwo = std::get<0>(loopPath2[j]);
        indexOne = std::get<1>(loopPath1[i]);
        indexTwo = std::get<1>(loopPath2[j]);

        if(this->nodeMatch(nodeOne, nodeTwo))
        {
            if(nodeOne->type != nodeType::Virtual)
            {
                alignedPairs.push_back(std::make_tuple(indexOne, indexTwo));
                //std::cout << "pushing i: " << indexOne << ", j: " << indexTwo << ", size: " << alignedPairs.size() << std::endl;
            }
            else
            {
                if(this->__loopGreedyContents(((virtualNode *)nodeOne)->contents,((virtualNode *)nodeTwo)->contents, alignedPairs, diff) != ErrorCode::SUCCESS)
                {
                    std::cerr << "Failed in virtual node content\n";
                    exit(1);
                } 
            }
            /*
            std::cout << "node matched: ";
            nodeOne->printNodeSimply(std::cout);
            std::cout << '\n';
            */
            i++; j++;
        }
        else
        {
            diff++;
            std::cout << "diverted at ";
            (std::get<0>(loopPath1[i - 1]))->printNodeSimply(std::cout);
            //nodeOne->printNodeSimply(std::cout);
            std::cout << " in i: " << i - 1 << " and j: " << j - 1 << std::endl;

        
            // if the matching hashmap is not filled, fill it up
            if(match.size() == 0 && virtualNodeMatch.size() == 0)
            {
                node *tmpNode;
                for(unsigned k = j; k < loopPath2.size(); k++)
                {
                    tmpNode = std::get<0>(loopPath2[k]);
                    if(tmpNode->type == nodeType::Virtual)
                    {
                        virtualNodeMatch[tmpNode->name].push_back(k);
                    }
                    else
                    {
                        match[tmpNode].push_back(k);
                    }
                }
            }

            j_start = j;
            while(i < loopPath1.size())
            {
                node *node2Match = std::get<0>(loopPath1[i]);
                unsigned indexOne = std::get<1>(loopPath1[i]);
                unsigned indexTwo;
                node *nodeMatched;
                //unsigned indexTwo;
                if(node2Match->type != nodeType::Virtual && match.find(node2Match) != match.end())
                {
                    // this is when it was found in the normal one
                    std::vector<unsigned> &vec = match[node2Match];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++)
                    {
                        if(vec[t] >= j_start)
                        {
                            j = vec[t];
                            nodeMatched = std::get<0>(loopPath2[j]);
                            indexTwo = std::get<1>(loopPath2[j]);
                            /*
                            std::cout << "node found: ";
                            nodeMatched->printNodeSimply(std::cout);
                            
                            std::cout << "\t" << indexTwo << std::endl;
                            */
                            found = true;
                            for(unsigned alpha = 0; alpha < t; alpha++)
                            { 
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found)
                    {
                        std::cout << "NODE NOT FOUND\n";
                        i++;
                    }
                    else
                    {
                        std::cout << "converged at ";
                        node2Match->printNodeSimply(std::cout);
                        std::cout << " in i: " << indexOne << " and j: " << indexTwo << std::endl;
                        alignedPairs.push_back(std::make_tuple(indexOne, indexTwo));
                        i++; j++;
                        break;
                    }
                }
                else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) == virtualNodeMatch.end())
                {
                    // this is when it was found in the virtual node
                    std::vector<unsigned> &vec = virtualNodeMatch[node2Match->name];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++)
                    {
                        if(vec[t] >= j_start)
                        {
                            j = vec[t];
                            found = true;
                            //alignedVN.push_back(std::make_tuple(i, j));
                            for(unsigned alpha = 0; alpha < t; alpha++)
                            {
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found)
                    {
                        i++;
                    }
                    else
                    {
                        std::cout << "converged at ";
                        node2Match->printNodeSimply(std::cout);
                        std::cout << " in i: " << indexOne << " and j: " << j << std::endl; 
                        i++; j++;
                        break;
                    }
                }
                else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) != virtualNodeMatch.end())
                {
                    // this is when it was not found as the virtual node
                    i++;
                }
                else
                {
                    // this is when it was not found as not a virtual node
                    i++;
                }
            }
        }
    }
    return ErrorCode::SUCCESS;
}


regexGraph Comparison::createRegex()
{
    std::vector<std::vector<node *>> paths{hpath, vpath};
    std::vector<node *> path;
    node *currNode;
    node *prev;
    std::set<node *> nodes;
    std::map<node *, std::set<node *>> edges;
    node *entryPoint = paths[0][0];

    for(int i = 0; i < paths.size(); i++) 
    {
        path = paths[i];
        prev = nullptr; 
        for(int j = 0; j < path.size(); j++) 
        {
            currNode = path[j];
            nodes.insert(currNode);
            if(prev != nullptr) 
            {
                if(isContained(prev, edges)) 
                {
                    edges[prev].insert(currNode);
                }
                else
                {
                    std::set<node *> dsts{currNode};
                    edges[prev] = dsts;
                }
            }
            prev = currNode;
        }
    }
    
    return regexGraph(entryPoint, nodes, edges);
}

ErrorCode Comparison::regEx(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    regexGraph regex = createRegex();
    return ErrorCode::SUCCESS;
}

/*
ErrorCode Comparison::regEx(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    //std::unordered_set<std::string> vertices;
    std::unordered_map<std::string, std::string> conversionTable;
    int index = 128;
    
    for(unsigned i = 0; i < hpath.size(); i++) 
    {
        std::string tmp = std::string(hpath[i]->name);
        auto got = conversionTable.find(tmp);
        if(got == conversionTable.end()) 
        {
            conversionTable[tmp] = std::to_string(index++);
        }
    }

    for(unsigned i = 0; i < vpath.size(); i++)
    {
        std::string tmp = std::string(vpath[i]->name);
        auto got = conversionTable.find(tmp);
        if(got == conversionTable.end()) 
        {
            conversionTable[tmp] = std::to_string(index++);
        }
    } 

    for(auto i = conversionTable.begin(); i != conversionTable.end(); i++)
    {
        std::cout << i->first << ", " << i->second << std::endl;
        insertAlpha(i->second);
    }
    //printAlpha();

    std::vector<std::string> path1, path2;
    std::vector<std::vector<std::string>> input;
    for(unsigned i = 0; i < hpath.size(); i++) 
    {
        path1.push_back(conversionTable[std::string(hpath[i]->name)]);
    } 
    for(unsigned i = 0; i < vpath.size(); i++) 
    {
        path2.push_back(conversionTable[std::string(vpath[i]->name)]);
    }
    input.push_back(path1);
    input.push_back(path2);
    
    std::vector<std::unordered_map<int, std::unordered_set<int>>> transition_table;
    
    int l = std::max(hpath.size(), vpath.size());
    std::cout << "l: " << l << std::endl;
    construct(input, l, transition_table);
    for(int i = 0; i < transition_table.size(); i++) {
        std::cout << "at " << i << std::endl;
        for(auto element: transition_table[i]) {
            std::cout << element.first << "->";
            for(auto itr = element.second.begin(); itr != element.second.end(); itr++) {
                std::cout << *itr << ", ";
            }
            std::cout << std::endl;
        }
    }

    return ErrorCode::SUCCESS;
}
*/
