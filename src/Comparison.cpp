
#include "Comparison.h"
using namespace std;
using namespace Tools;
//using namespace regex;

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

Comparison::~Comparison()
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

// this does not create a virtual nodes
void Comparison::greedyApproach(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    std::vector<node *>& path1 = hpath;
    std::vector<node *>& path2 = vpath;

    unsigned i = 0; unsigned j = 0;
    std::map<node *, std::vector<unsigned>> match;
    unsigned j_start;

    while(i < path1.size() && j < path2.size()) {
        if(path1[i] == path2[j]) {
            alignedPairs.push_back(std::make_tuple(i, j));
            //std::cout << "adding (top) :" << i << ", " << j << std::endl;
            i++; j++;
        } else {
            diff++;
            std::cout << "diverted at ";
            path1[i - 1]->printNodeSimply(std::cout);
            std::cout << " in i: " << i - 1 << " and j: " << j - 1 << std::endl;
            // preparing the hashmap
            if(match.size() == 0) {
                for(unsigned k = j; k < path2.size(); k++) {
                    //std::cout << "adding :" << *path2[k] <<std::endl;
                    match[path2[k]].push_back(k);
                } 
            }

            j_start = j;
            // search for each node in path 1 if it exists in path2
            while(i < path1.size()) {
                if(match.find(path1[i]) == match.end()) {
                    //std::cout << "did not find for " << *(path1[i]) << "i: " << i << std::endl;
                    i++;
                    //std::cout << "i: " << i << std::endl;
                } else {
                    //std::cout << "found for " << *(path1[i]) << "i: " << i << std::endl;
                    vector<unsigned>& vec = match[path1[i]];
                    bool found = false;
                    unsigned size = vec.size();
                    //std::cout << vec[0] << ',' << j_start << std::endl;
                    for(unsigned t = 0; t < size; t++) {
                        if(vec[t] >= j_start) {
                            j = vec[t];
                            found = true;
                            //std::cout << "adding " << i << "," << j << std::endl;
                            alignedPairs.push_back(make_tuple(i, j));
                            for(unsigned alpha = 0; alpha < t; alpha++) {
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found) {
                        i++;
                    } else {
                        cout << "converged at ";
                        path1[i]->printNodeSimply(cout);
                        cout << " in i: " << i << " and j: " << j << endl;
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
    if(a->type == nodeType::Virtual && b->type == nodeType::Virtual) {
        virtualNode *va = (virtualNode *)a, *vb = (virtualNode *)b;
        if(va->head == vb->head) return true;
        else return false;
    } else if(a->type == nodeType::Virtual && b->type == nodeType::Virtual) {
        return false;
    } else {
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

    while(i < content1.size() && j < content2.size()) {
        nodeOne = std::get<0>(content1[i]);
        nodeTwo = std::get<0>(content2[j]);
        indexOne = std::get<1>(content1[i]);
        indexTwo = std::get<1>(content2[j]);
        if(this->nodeMatch(nodeOne, nodeTwo)) {
            if(nodeOne->type != nodeType::Virtual) {
                alignedPairs.push_back(std::make_tuple(indexOne, indexTwo));
                //std::cout << "pushing i: " << indexOne << ", j: " << indexTwo << ", size: " << alignedPairs.size() << std::endl;
            } else {
                if(this->__loopGreedyContents(((virtualNode *)nodeOne)->contents, ((virtualNode *)nodeTwo)->contents, alignedPairs, diff) != ErrorCode::SUCCESS) {
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
        } else {
            aligned = false;
            diff++;
            cout << "diverted at ";
            (get<0>(content1[i - 1]))->printNodeSimply(cout);
            //nodeOne->printNodeSimply(std::cout);
            cout << " in i: " << i - 1 << " and j: " << j - 1 << endl;

        
            // if the matching hashmap is not filled, fill it up
            if(match.size() == 0 && virtualNodeMatch.size() == 0) {
                node *tmpNode;
                for(unsigned k = j; k < content2.size(); k++) {
                    tmpNode = std::get<0>(content2[k]);
                    if(tmpNode->type == nodeType::Virtual) {
                        virtualNodeMatch[tmpNode->name].push_back(k);
                    } else {
                        match[tmpNode].push_back(k);
                    }
                }
            }

            j_start = j;
            while(i < content1.size()) {
                node *node2Match = std::get<0>(content1[i]);
                unsigned indexOne = std::get<1>(content1[i]);
                unsigned indexTwo;
                node *nodeMatched;
                //unsigned indexTwo;
                if(node2Match->type != nodeType::Virtual && match.find(node2Match) != match.end()) {
                    // this is when it was found in the normal one
                    vector<unsigned> &vec = match[node2Match];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++) {
                        if(vec[t] >= j_start) {
                            j = vec[t];
                            nodeMatched = get<0>(content2[j]);
                            indexTwo = get<1>(content2[j]);
                            //std::cout << "node found: ";
                            //nodeMatched->printNodeSimply(std::cout);
                            
                            //std::cout << "\t" << indexTwo << std::endl;
                            found = true;
                            for(unsigned alpha = 0; alpha < t; alpha++) { 
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found) {
                        cout << "NODE NOT FOUND\n";
                        i++;
                    } else {
                        cout << "converged at ";
                        node2Match->printNodeSimply(cout);
                        cout << " in i: " << indexOne << " and j: " << indexTwo << endl;
                        alignedPairs.push_back(make_tuple(indexOne, indexTwo));
                        //std::cout << "pushing i: " << indexOne << ", j: " << indexTwo << ", size: " << alignedPairs.size() << std::endl;
                        aligned = true;
                        i++; j++;
                        break;
                    }
                } else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) == virtualNodeMatch.end()) {
                    // this is when it was found in the virtual node
                    vector<unsigned> &vec = virtualNodeMatch[node2Match->name];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++) {
                        if(vec[t] >= j_start) {
                            j = vec[t];
                            found = true;
                            //alignedVN.push_back(std::make_tuple(i, j));
                            for(unsigned alpha = 0; alpha < t; alpha++) {
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found) {
                        i++;
                    } else {
                        cout << "converged at ";
                        node2Match->printNodeSimply(cout);
                        cout << " in i: " << indexOne << " and j: " << indexTwo << endl; 
                        i++; j++;
                        aligned = true;
                        if(this->__loopGreedyContents(((virtualNode *)node2Match)->contents, ((virtualNode *)nodeTwo)->contents, alignedPairs, diff) != ErrorCode::SUCCESS) {
                            cerr << "something wrong with the __loopGreedyContents\n";
                            exit(1);
                        }
                        break;
                    }
                } else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) != virtualNodeMatch.end()) {
                    // this is when it was not found as the virtual node
                    i++;
                } else {
                    // this is when it was not found as not a virtual node
                    i++;
                }
            }


            if(aligned == false) {
                node *tmpNode = get<0>(content1[0]);
                cout << "converged at ";
                tmpNode->printNodeSimply(cout);
                cout << "at the end\n";
            }

        }
    }
    return ErrorCode::SUCCESS;
}

/*
 * this is finding the closest common points as a point of convergence using greedy approach
 * virtual nodes are made using the nodes that are traversed multiple times
 * you should look over this and make the code simpler
 */
ErrorCode Comparison::loopGreedyApproach(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    /*
    std::vector<node *>& path1 = hpath;
    std::vector<node *>& path2 = vpath;
    */

    /* 
     * step 1. get the matching pairs of the nodes 
     */

    unsigned i = 0, j = 0; // these only consider the match at the surface
    map<node *, vector<unsigned>> match;
    map<string, vector<unsigned>> virtualNodeMatch;

    node *nodeOne;
    node *nodeTwo;
    unsigned indexOne;
    unsigned indexTwo;
    unsigned j_start;

    while(i < loopPath1.size() && j < loopPath2.size()) {
        nodeOne = get<0>(loopPath1[i]); 
        nodeTwo = get<0>(loopPath2[j]);
        indexOne = get<1>(loopPath1[i]);
        indexTwo = get<1>(loopPath2[j]);
        // if the nodes matches
        if(this->nodeMatch(nodeOne, nodeTwo)) {
            // if they are not virtual nodes
            if(nodeOne->type != nodeType::Virtual) {
                alignedPairs.push_back(make_tuple(indexOne, indexTwo));
                //std::cout << "pushing i: " << indexOne << ", j: " << indexTwo << ", size: " << alignedPairs.size() << std::endl;
            } else {
                if(this->__loopGreedyContents(((virtualNode *)nodeOne)->contents,((virtualNode *)nodeTwo)->contents, alignedPairs, diff) != ErrorCode::SUCCESS) {
                    cerr << "Failed in virtual node content\n";
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
            cout << "diverted at ";
            (get<0>(loopPath1[i - 1]))->printNodeSimply(cout);
            //nodeOne->printNodeSimply(std::cout);
            cout << " in i: " << i - 1 << " and j: " << j - 1 << endl;

        
            // if the matching hashmap is not filled, fill it up
            if(match.size() == 0 && virtualNodeMatch.size() == 0) {
                node *tmpNode;
                for(unsigned k = j; k < loopPath2.size(); k++) {
                    tmpNode = get<0>(loopPath2[k]);
                    if(tmpNode->type == nodeType::Virtual) {
                        virtualNodeMatch[tmpNode->name].push_back(k);
                    } else {
                        match[tmpNode].push_back(k);
                    }
                }
            }

            j_start = j;
            while(i < loopPath1.size()) {
                node *node2Match = get<0>(loopPath1[i]);
                unsigned indexOne = get<1>(loopPath1[i]);
                unsigned indexTwo;
                node *nodeMatched;
                //unsigned indexTwo;
                if(node2Match->type != nodeType::Virtual && match.find(node2Match) != match.end()) {
                    // this is when it was found in the normal one
                    vector<unsigned> &vec = match[node2Match];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++) {
                        if(vec[t] >= j_start) {
                            j = vec[t];
                            nodeMatched = get<0>(loopPath2[j]);
                            indexTwo = get<1>(loopPath2[j]);
                            /*
                            std::cout << "node found: ";
                            nodeMatched->printNodeSimply(std::cout);
                            
                            std::cout << "\t" << indexTwo << std::endl;
                            */
                            found = true;
                            for(unsigned alpha = 0; alpha < t; alpha++) { 
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found) {
                        cout << "NODE NOT FOUND\n";
                        i++;
                    } else {
                        cout << "converged at ";
                        node2Match->printNodeSimply(cout);
                        cout << " in i: " << indexOne << " and j: " << indexTwo << endl;
                        alignedPairs.push_back(std::make_tuple(indexOne, indexTwo));
                        i++; j++;
                        break;
                    }
                } else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) == virtualNodeMatch.end()) {
                    // this is when it was found in the virtual node
                    vector<unsigned> &vec = virtualNodeMatch[node2Match->name];
                    bool found = false;
                    unsigned size = vec.size();
                    for(unsigned t = 0; t < size; t++) {
                        if(vec[t] >= j_start) {
                            j = vec[t];
                            found = true;
                            //alignedVN.push_back(std::make_tuple(i, j));
                            for(unsigned alpha = 0; alpha < t; alpha++) {
                                vec.erase(vec.begin());
                            }
                            break;
                        }
                    }

                    if(!found) {
                        i++;
                    } else {
                        cout << "converged at ";
                        node2Match->printNodeSimply(cout);
                        cout << " in i: " << indexOne << " and j: " << j << endl; 
                        i++; j++;
                        break;
                    }
                } else if(node2Match->type == nodeType::Virtual && virtualNodeMatch.find(node2Match->name) != virtualNodeMatch.end()) {
                    // this is when it was not found as the virtual node
                    i++;
                } else {
                    // this is when it was not found as not a virtual node
                    i++;
                }
            }
        }
    }
    return ErrorCode::SUCCESS;
}


/*
regexGraph Comparison::createRegex()
{
    std::vector<std::vector<node *>> paths{hpath, vpath};
    std::vector<node *> path;
    node *currNode;
    node *prev;
    std::set<node *> nodes;
    std::map<node *, std::set<node *>> edges;
    node *entryPoint = paths[0][0];

    for(unsigned i = 0; i < paths.size(); i++) 
    {
        path = paths[i];
        prev = nullptr; 
        for(unsigned j = 0; j < path.size(); j++) 
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

static bool cmpTuple(const tuple<unsigned, unsigned> &t1, const tuple<unsigned, unsigned> &t2)
{
    return get<0>(t1) < get<0>(t2); 
}

ErrorCode Comparison::regEx(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff)
{
    regexGraph regex = createRegex();
    cout << "done with creating regex\n";
    //cout << "printing regexGraph\n";
    //cout << regex;
    regex.visualize("file.txt");
    set<int> baseColors1;
    set<int> baseColors2;
    unsigned index = 0;
    vector<regexNode> regexHPath = pathAnalysis(hpath, regex, baseColors1, index);   
    cout << "printing hpath in regex considered form\n";
    pathPrint(regexHPath);
    index = 0;
    vector<regexNode> regexVPath = pathAnalysis(vpath, regex, baseColors2, index);
    vector<Interval> points; // this shows where it diverged and converged
    cout << "done with analysis of the paths\n"; 
    
    regexAlignment(regexHPath, regexVPath, alignedPairs, diff, points);
    cout << "done with alignment\n";
    sort(points.begin(), points.end(), cmpInterval);
    cout << "printing points\n";
    cout << points << endl;

    sort(alignedPairs.begin(), alignedPairs.end(), cmpTuple);

    for(unsigned i = 0; i < points.size(); i++)
    {
        cout << "diverged at " << *hpath[points[i].start] << endl 
            << "reconverged at " << *hpath[points[i].end] << endl;
    }

    cout << "printing alignedPairs\n";
    for(unsigned i = 0; i < alignedPairs.size(); i++)
    {
        cout << alignedPairs[i] << ", ";
    }
    cout << endl;
    
    return ErrorCode::SUCCESS;
}
*/
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

bool Comparison::stopCondition(node *n, node *dp, set<node *>& pd) {
    if(n->type == nodeType::Virtual) {
        //virtualNode *vn = (virtualNode *)n;
        if(!nodeMatch(n, dp) && isContained(n, pd)) {
            return true;
        } 
    } else {
        if(isContained(n, pd))
            return true;
    }
    return false;
}

void Comparison::printConvergence(node *cp, node *cpOne, node *cpTwo, unsigned i, unsigned j) 
{
    if(cp->type == nodeType::Virtual) {
        cp = ((virtualNode *)cp)->head;
        assert(cpOne->type == nodeType::Virtual && cpTwo->type == nodeType::Virtual);
        virtualNode *vnOne = (virtualNode *)cpOne, *vnTwo = (virtualNode *)cpTwo;
        while(cpOne->type == nodeType::Virtual) {
            cpOne = get<0>(vnOne->contents[0]);
            i = get<1>(vnOne->contents[0]);
            if(cpOne->type == nodeType::Virtual) {
                vnOne = (virtualNode *)cpOne;
            }
        }
        while(cpTwo->type == nodeType::Virtual) {
            cpTwo = get<0>(vnTwo->contents[0]);
            j = get<1>(vnTwo->contents[0]);
            if(cpTwo->type == nodeType::Virtual) {
                vnTwo = (virtualNode *)cpTwo;
            }
        }

        cout << "converged at " << *cp << "in i: " 
            << i << " and j: " << j << endl;
    } else {
        cout << "converged at " << *cp << "in i: " 
            << i << " and j: " << j << endl;
    }
    return;
}

void Comparison::printConvergence(node *cp)
{
    while(cp->type == nodeType::Virtual) 
        cp = ((virtualNode *)cp)->head;
    cout << "converged at " << *cp;
    return;
}

void Comparison::printDivergence(node *dp, node *dpOne, node *dpTwo, unsigned i, unsigned j)
{
    if(dp->type == nodeType::Virtual) { 
        dp = ((virtualNode *)dp)->head;
        assert(dpOne->type == nodeType::Virtual && dpTwo->type == nodeType::Virtual);
        virtualNode *vnOne = (virtualNode *)dpOne, *vnTwo = (virtualNode *)dpTwo;
        unsigned lastOne = get<1>(vnOne->contents[vnOne->contents.size() - 1]); 
        unsigned lastTwo = get<1>(vnTwo->contents[vnTwo->contents.size() - 1]); 
        cout << "diverted at " << *dp << " in i: " 
            << lastOne << " and j: " << lastTwo << endl;
    } else {
        cout << "diverted at " << *dp << " in i: " 
            << i << " and j: " << j << endl;
    }
    return;
}

/* 
 * the below method is creating a post dominator
 */
ErrorCode Comparison::postDominatorApproach(vector<tuple<unsigned, unsigned>> &alignedPairs, int &diff, vector<tuple<node *, unsigned>>& tlp1, vector<tuple<node *, unsigned>>& tlp2) {
    vector<node *> lp1{}, lp2{};
    for(tuple<node *, unsigned> ln: tlp1) {
        lp1.push_back(get<0>(ln));
    }
    for(tuple<node *, unsigned> ln: tlp2) {
        lp2.push_back(get<0>(ln));
    }

    /*
    cout << "printing the paths\npath1\n";
    for(auto node: lp1) {
        cout << *node << endl;
    }
    cout << "path2\n";
    for(auto node: lp2) {
        cout << *node << endl;
    }
    cout << endl << endl;
    */
    
    subgraph *sg = new subgraph(lp1, lp2);
    //cout << "i made a subgraph\n";
    map<node *, set<node *>> pd = sg->findPostDominators();
    //cout << "found post dominators\n";
    //cout << pd << endl;

    unsigned i = 0, j = 0, indexOne = 0, indexTwo = 0, prevOne, prevTwo;
    node *nodeOne = nullptr, *nodeTwo = nullptr, *dp = nullptr, *tmpOne, *tmpTwo, *prevNodeOne, *prevNodeTwo;
    while(i < tlp1.size() && j < tlp2.size()) {
        prevNodeOne = nodeOne;
        prevNodeTwo = nodeTwo;
        nodeOne = get<0>(tlp1[i]); 
        nodeTwo = get<0>(tlp2[j]);
        prevOne = indexOne;
        prevTwo = indexTwo;
        indexOne = get<1>(tlp1[i]);
        indexTwo = get<1>(tlp2[j]);
        if(nodeOne->type == nodeType::Virtual) {
            tmpOne = nodeOne;
            nodeOne = (node *)(sg->virtualNodes[((virtualNode *)nodeOne)->head]);
        }
        if(nodeTwo->type == nodeType::Virtual) {
            tmpTwo = nodeTwo;
            nodeTwo = (node *)(sg->virtualNodes[((virtualNode *)nodeTwo)->head]);
        }
        //cout << "nodeOne: " << *nodeOne << endl;
        //cout << "nodeTwo:" << *nodeTwo << endl;
        if(nodeMatch(nodeOne, nodeTwo)) {
            if(nodeOne->type != nodeType::Virtual) {
                alignedPairs.push_back(make_tuple(indexOne, indexTwo));
            } else {
                if(postDominatorApproach(alignedPairs, diff, ((virtualNode *)tmpOne)->contents, ((virtualNode *)tmpTwo)->contents) != ErrorCode::SUCCESS) {
                    cerr << "Failed in virtual node content\n";
                    exit(1);
                }
            }
            i++; j++;
        } else {
            dp = get<0>(tlp1[i - 1]);
            if(dp->type == nodeType::Virtual) {
                dp = sg->virtualNodes[((virtualNode *)dp)->head];
            }
            //cout << "indexOne: " << indexOne << ", i: " << i << ", indexTwo: " << indexTwo << ", j: " << j << endl;
            printDivergence(dp, prevNodeOne, prevNodeTwo, prevOne, prevTwo);

            diff++;

            set<node *> &p = pd[dp];

            bool end = false;
            while(!stopCondition(nodeOne, dp, p)) {
                if(i == tlp1.size() - 1) {
                    end = true;
                    break;
                }
                nodeOne = get<0>(tlp1[++i]);
                if(nodeOne->type == nodeType::Virtual) {
                    tmpOne = nodeOne;
                    nodeOne = (node *)(sg->virtualNodes[((virtualNode *)nodeOne)->head]);
                } 
                indexOne = get<1>(tlp1[i]);
            } 

            if(end) {
                node *endNode = get<0>(tlp1[0]);
                printConvergence(endNode); 
                cout << " at the end of the loop iteration\n";
                if(endNode->type == nodeType::ePoint) {
                    cerr << "Error: the last point was entry point of the function\nlast divergence point: " << *dp << endl;
                    exit(1);
                }

                break;
            }

            while(!stopCondition(nodeTwo, dp, p)) {
                nodeTwo = get<0>(tlp2[++j]);
                if(nodeTwo->type == nodeType::Virtual) {
                    tmpTwo = nodeTwo;
                    nodeTwo = (node *)(sg->virtualNodes[((virtualNode *)nodeTwo)->head]);
                } 
                indexTwo = get<1>(tlp2[j]); 
            }
            // I am asserting here 
            assert(nodeMatch(nodeOne, nodeTwo));
            printConvergence(nodeOne, nodeOne, nodeTwo, indexOne, indexTwo);
        }
    }

    delete sg;
    //cout << "returning\n";
    return ErrorCode::SUCCESS;
}


