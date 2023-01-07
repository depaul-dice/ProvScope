// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

/* 
 * I copied the code from https://www.techiedelight.com/print-keys-values-map-cpp/ to print out the map
 */
#include "funcTrace.h"
using namespace std;
using namespace Tools;

/*
funcTraceElement::funcTraceElement(const string& funcName)
    :funcName(funcName)
{
}

funcTraceElement::funcTraceElement(funcTrace& tmp)
    :funcName(tmp.funcName), ft(&tmp), hv(tmp.deepHV)
{
}

bool funcTraceElement::operator == (const funcTraceElement& tmp) 
{
    return this->hv == tmp.hv;
}
*/

funcTrace::~funcTrace()
{
    for(unsigned i = 0; i < __funcTrace.size(); i++)
    {
        delete __funcTrace[i];
    }

    for(unsigned i = 0; i < virtualNodes.size(); i++)
    {
        delete virtualNodes[i];
    }
}

funcTrace::funcTrace(string funcName)
    :funcName(funcName), virtualNodes(), DEBUG(0)
{
    memset(surfaceDigest, 0, 20);
    memset(deepDigest, 0, 20);
}

void funcTrace::push_back(funcTrace *tmp)
{
    this->__funcTrace.push_back(tmp);
}

bool funcTrace::deepEqual(const funcTrace &tmp) 
{
    //return this->deepHV == tmp.deepHV;
    for(int i = 0; i < 20; i++)
    {
        if(deepDigest[i] != tmp.deepDigest[i])
            return false;
    }
    return true;
}

bool funcTrace::operator == (const funcTrace &tmp)
{
    for(int i = 0; i < 20; i++)
    {
        if(surfaceDigest[i] != tmp.surfaceDigest[i])
            return false;
    }
    return true;
}

funcTrace* funcTrace::operator [] (size_t idx)
{
    return __funcTrace[idx];
}

const funcTrace* funcTrace::operator [] (size_t idx) const
{
    return __funcTrace[idx];
}

string funcTrace::getFuncName(void)
{
    return funcName;
}

vector<funcTrace *> funcTrace::getFuncTrace(void)
{
    return __funcTrace;
}

void funcTrace::print_digest(unsigned char *digest)
{
    for(int i = 0; i < 20; i++)
    {
        printf("%d ", digest[i]);
    }
    printf("\n");
}

unsigned char *funcTrace::updateHash(void)
{
    if(__funcTrace.size() == 0)
    {
        return deepDigest;
    }

    SHA_CTX surfaceHV;
    SHA_CTX deepHV;
    unsigned char *tmp;
    SHA1_Init(&surfaceHV);
    SHA1_Init(&deepHV);

    for(unsigned i = 0; i < __funcTrace.size(); i++)
    {
        SHA1_Update(&surfaceHV, __funcTrace[i]->getFuncName().c_str(), __funcTrace[i]->getFuncName().size());
        tmp = __funcTrace[i]->updateHash();
        SHA1_Update(&deepHV, tmp, 20);
    }
    SHA1_Final(surfaceDigest, &surfaceHV);
    SHA1_Update(&deepHV, surfaceDigest, 20);
    SHA1_Final(deepDigest, &deepHV);
    return deepDigest;
}

std::ostream& operator << (std::ostream& os, funcTrace const& ft)
{
    os << "funcName: " << ft.funcName << '\n';
    for(unsigned i = 0; i < ft.__funcTrace.size(); i++)
    {
        os << ft.__funcTrace[i]->funcName << '\n';
    }
    os << '\n';

    return os;
}

std::ostream& operator << (std::ostream& os, vector<funcTrace *> const& tmp)
{
    for(unsigned i = 0; i < tmp.size(); i++)
    {
        os << tmp[i]->funcName << '\n';
    }
    os << '\n';
    return os;
}

void funcTrace::__printRecursiveFT(ostream& os, int tabs, int &count)
{
    for(unsigned i = 0; i < this->__funcTrace.size(); i++)
    {
        os << ++count << "\t:";
        for(int j = 0; j < tabs; j++)
        {
            os << '\t';
        }
        os << this->__funcTrace[i]->funcName << '\n';
        this->__funcTrace[i]->__printRecursiveFT(os, tabs + 1, count);
    }
}

void funcTrace::printRecursiveFT(ostream& os)
{
    int count = 0;
    int tabs = 0;
    for(unsigned i = 0; i < this->__funcTrace.size(); i++) {
        os << ++count << "\t:" << this->__funcTrace[i]->funcName << '\n';
        this->__funcTrace[i]->__printRecursiveFT(os, tabs + 1, count);
    }
    /*
    os << *this << endl;
    for(unsigned i = 0; i < this->__funcTrace.size(); i++)
    {
        //cout << *(this->__funcTrace[i]) << endl;
        this->__funcTrace[i]->printRecursiveFT();
    } 
    */
}

void funcTrace::printRecursiveFTFormat(ostream& os, int tabs) {
    for(unsigned i = 0; i < this->__funcTrace.size(); i++) {
        os << tabs << ":" << this->__funcTrace[i]->funcName << '\n';
        this->__funcTrace[i]->printRecursiveFTFormat(os, tabs + 1);
    }
}

unsigned popTillLastBranch(vector<node *> &path, vector<unsigned> &branch, unsigned index, ErrorCode &ec)
{
    if(path.size() == 0)
    {
        unsigned lBranch = branch.back();
        branch.pop_back();
        branch.push_back(lBranch + 1);
        assert(index == 0);
        return index;
    }

    while(branch.back() == (unsigned)(path.back()->numOEdges - 1) || path.back()->numOEdges == 0 )
    {
        path.pop_back(); 
        branch.pop_back();
        index--;
        if(path.size() == 0)
        {
            //std::cerr << "do not have a matching path" << endl;
            ec = ErrorCode::NOPATH;
            return index;
        }
    }

    //assert(branch.back() < (unsigned)(path.back()->numOEdges - 1));
    if(branch.back() >= (unsigned)(path.back()->numOEdges - 1))
    {
        std::cerr << "branch.size(): " << branch.size() << "vs. "
            << "path.size(): " << path.size() << '\n';
        for(unsigned i = 0; i < branch.size(); i++)
        {
            std::cerr << branch[i] << ',';
        }
        std::cerr << endl;
        for(unsigned i = 0; i < path.size(); i++)
        {
            std::cerr << *path[i] << endl;
        }

        std::cerr << "branch.back(): " << branch.back() << 
            " < (unsigned)(path.back()->numOEdges - 1): " <<             
            (unsigned)(path.back()->numOEdges - 1) << 
            " failed\n" << 
            *(path.back()) << '\n' <<
            "aborting...\n"; 
        exit(1); 
    } 
    unsigned lBranch = branch.back(); 
    branch.pop_back(); 
    branch.push_back(lBranch + 1); 

    ec = ErrorCode::SUCCESS;
    return index;
}

vector<vector<node *>> getAllPossiblePaths(funcTrace *ft, cfg_t *cfg)
{
    vector<vector<node *>> rv;
    vector<node *> path {};
    vector<unsigned> branch {};
    path.push_back(cfg->ep);   
    branch.push_back(0);
    unsigned index = 0;
    string currStr;
    vector<funcTrace *> currft = ft->getFuncTrace();
    node *curr;
    node *next;
    funcNode *funcNext;
    ErrorCode ec = ErrorCode::SUCCESS;
    //bool found;
    unsigned num = 0;

    while(true)
    {
        curr = path.back();
        //num = branch.back();
        //cout << num << endl << *curr << endl;
        next = curr->oEdges[num].ptr;
        if(branch.size() != path.size())
        {
            std::cerr << "size of branch and size of path were different\n";
            std::cerr << "branch.size(): " << 
                branch.size() << " vs. path.size(): " <<
                path.size() << endl;
            exit(1);
        }

        if(next->type == nodeType::funcCall)
        {
            funcNext = (funcNode *)next;
            if(index >= currft.size() || funcNext->funcName != currft[index]->getFuncName())
            {
                index = popTillLastBranch(path, branch, index, ec);
                if(ec == ErrorCode::NOPATH)
                {
                    return rv;
                }
                assert(branch.size() > 0);
                num = branch.back();
            }
            else
            {
                path.push_back(next);
                branch.push_back(0);
                index++;
                num = 0;
                //branch.push_back(0);
            }
        }
        else
        {
            if(next->type != nodeType::retCall)
            {
                std::cerr << "nodeType was not retCall at the end:" << next->type << "\n";
                std::cerr << "aborting...\n";
                exit(1);
            }
            //assert(next->type == nodeType::retCall);
            if(index == currft.size())
            {
                path.push_back(next);
                vector<node *> pathCopy = path;

                rv.push_back(pathCopy);
                path.pop_back();
                index = popTillLastBranch(path, branch, index, ec);
                if(ec == ErrorCode::NOPATH)
                {
                    return rv;
                }
                num = branch.back();
            }
            else
            {
                index = popTillLastBranch(path, branch, index, ec);
                if(ec == ErrorCode::NOPATH)
                {
                    return rv;
                }
                assert(branch.size() > 0);
                num = branch.back();
            }
        }
        
    }

    return rv;
}

vector<node *> getPossiblePath(funcTrace *ft, cfg_t *cfg)
{
    vector<node *> path;
    vector<unsigned> branch;
    path.push_back(cfg->ep);   
    branch.push_back(0);
    unsigned index = 0;
    unsigned biggestIndex = 0;
    string currStr;
    vector<funcTrace *> currft = ft->getFuncTrace();
    node *curr;
    node *next;
    funcNode *funcNext;
    ErrorCode ec = ErrorCode::SUCCESS;
    //bool found;
    unsigned num = 0;

    while(true)
    {
        curr = path.back();
        //num = branch.back();

        next = curr->oEdges[num].ptr;
        assert(next != nullptr);
        if(index > biggestIndex)
            biggestIndex = index;

        if(next->type == nodeType::funcCall)
        {
            funcNext = (funcNode *)next;
            if(index >= currft.size() || funcNext->funcName != currft[index]->getFuncName())
            {
                index = popTillLastBranch(path, branch, index, ec);
                if(ec == ErrorCode::NOPATH)
                {
                    std::cerr << "DO NOT have a matching path\nFunction Name: " << ft->getFuncName() << endl;
                    ft->print_currFT(".log.txt");
                    std::cerr << currft[biggestIndex]->getFuncName() << ',' << biggestIndex << endl;
                    exit(1);
                }
                assert(branch.size() > 0);
                num = branch.back();
            }
            else
            {
                path.push_back(next);
                branch.push_back(0);
                index++;
                num = 0;
                //branch.push_back(0);
            }
        }
        else
        {
            if(next->type != nodeType::retCall)
            {
                std::cerr << "nodeType was not retCall at the end:" << next->type << "\n";
                std::cerr << "aborting...\n";
                exit(1);
            }
            //assert(next->type == nodeType::retCall);
            if(index == currft.size())
            {
                path.push_back(next);
                return path;
            }
            else
            {
                index = popTillLastBranch(path, branch, index, ec);
                if(ec == ErrorCode::NOPATH)
                {
                    std::cerr << "DO NOT have a matching path\nFunction Name: " << ft->getFuncName() << endl;
                    ft->print_currFT(".log.txt");
                    std::cerr << currft[biggestIndex]->getFuncName() << ',' << biggestIndex << endl;
                    exit(1);
                }
                assert(branch.size() > 0);
                num = branch.back();
            }
        }
        
    }

    return path;
}

void funcTrace::printPath(std::ostream& os, vector<node *> path)
{
    for(unsigned i = 0; i < path.size(); i++)
    {
        path[i]->printNodeSimply(os);
        os << endl;
        //os << *path[i] << endl;
    }
}

void funcTrace::printLooppath(std::ostream& os, vector<std::tuple<node *, unsigned>>& loopPath, unsigned numTab)
{
    string tabs("");
    for(unsigned i = 0; i < numTab; i++)
    {
        tabs += '\t';
    }

    node *curr;
    int index;
    // you need to do this recursively
    for(unsigned i = 0; i < loopPath.size(); i++)
    {
        curr = std::get<0>(loopPath[i]);
        index = std::get<1>(loopPath[i]);
        os << tabs;
        curr->printNodeSimply(os);
        os << ':' << index << endl;
        if(curr->type == nodeType::Virtual)
        {
            this->printLooppath(os, ((virtualNode *)curr)->contents, numTab + 1);
        }
    }
}

/*
 * this function is used when you are creating the looppath
 * you go through each nodes and consider it the loop when you go through the same node
 */
vector<std::tuple<node *, unsigned>> funcTrace::cognizeLoops(vector<node *>& originalPath)
{
    //vector<node *> loopPath;
    vector<std::tuple<node *, unsigned>> loopPath; // the second element is index
    map<node *, unsigned> pastNodes;
    virtualNode *vn;
    node *curr;
    node *tmp;
    unsigned prev;
    unsigned index = 0;

    for(unsigned origIndex = 0; origIndex < originalPath.size(); origIndex++, index++) {
        curr = originalPath[origIndex];
        // if curr is not found in the pastNodes
        if(pastNodes.find(curr) == pastNodes.end()) {
            //loopPath.push_back(curr);
            loopPath.push_back(make_tuple(curr, origIndex));
            pastNodes[curr] = index;
        } else {
            // else if curr is found in the pastNodes
            prev = pastNodes[curr];
            // you want to create a tmpVec and insert all those in the index of loopPath into tmpVec
            // below is the part that copies at once
            vector<tuple<node *, unsigned>> newVec = vector<tuple<node *, unsigned>>(loopPath.begin() + prev, loopPath.end());
           

            // you need to delete all the smaller things from pastNodes
            for(unsigned i = prev; i < index; i++) {
                tmp = get<0>(loopPath.back());
                loopPath.pop_back();
                pastNodes.erase(tmp); // you need to delete the elements from hashmap too
               
                /*
                if(tmp->type == nodeType::Virtual) {
                    delete (virtualNode *)tmp;
                }
                */
            }
            index = prev + 1;
            vn = new virtualNode(newVec, curr->name); // and creating the virtual node here
            virtualNodes.push_back(vn);
            pastNodes[curr] = index;
           
            loopPath.push_back(make_tuple((node *)vn, (unsigned)-1));
            loopPath.push_back(make_tuple(curr, origIndex)); 
        }
    }
    return loopPath;
}

void funcTrace::ftcmp(funcTrace *ft2, map<string, cfg_t *> &cfgs, long &time, int &diff)
{
    /*
    if(DEBUG)
    {
        cout << "we are at " << this->funcName << endl; 
    }
    */
    if(this->deepEqual(*ft2))
    {
        /*
        if(DEBUG)
        {
            cout << "these were deeply the same" << endl;
            this->print_digest(this->deepDigest);
        }
        */
        return;
    }
    if(*this == *ft2)
    {
        //cout << "these were at the surface the same" << endl;
        assert(this->__funcTrace.size() == ft2->getFuncTrace().size());

        for(unsigned i = 0; i < this->__funcTrace.size(); i++)
        {
            this->__funcTrace[i]->ftcmp(ft2->getFuncTrace()[i], cfgs, time, diff);
        }
        return;
    } 

    //funcTrace *currft1, *currft2;
    cfg_t *cfg = cfgs[this->funcName];

    auto tik = std::chrono::steady_clock::now();
    vector<node *> path1 = getPossiblePath(this, cfg); 
    vector<node *> path2 = getPossiblePath(ft2, cfg);

    vector<std::tuple<node *, unsigned>> loopPath1 = this->cognizeLoops(path1);
    vector<std::tuple<node *, unsigned>> loopPath2 = this->cognizeLoops(path2);
    /*
    cout << "\n\nNow we are printing a simple path\n";
    for(unsigned i = 0; i < path1.size(); i++)
    {
        path1[i]->printNodeSimply(cout); 
        cout << endl;
    }
    cout << "\n\nNow we are printing loopPath\n";
    this->printLooppath(cout, loopPath1, 0);
    this->printLooppath(cout, loopPath2, 0);
    */
    
    auto tok = std::chrono::steady_clock::now();
#ifdef PRINTPOSSIBLEPATH 
    cout << "printing paths\n";
    printPath(cout, path2);
    printPath(cout, path1);
#endif
    time = time + std::chrono::duration_cast<std::chrono::microseconds>(tok - tik).count();

    //vector<std::tuple<unsigned, unsigned>> tmpPairs;

    Comparison c(path1, path2, loopPath1, loopPath2);
    this->__ftcmp(LOOPGREEDY, c, cfgs, ft2, diff, time);
}

void funcTrace::__ftcmp(int which, Comparison &c, map<string, cfg_t *> &cfgs, funcTrace *ft2, int &diff, long &time) 
{
    switch (which) {
        case GREEDY:
            greedyContent(c, diff, cfgs, ft2, time);
            break;
        case EDITDISTANCE:
            greedyContent(c, diff, cfgs, ft2, time);
            break;
        case LOOPGREEDY:
            loopGreedyContent(c, diff, cfgs, ft2, time);
            break;
        case POSTDOMINATOR:
            postDominatorContent(c, diff, cfgs, ft2, time);
            break;
        /*
        case REGEX:
            regExContent(c, diff, cfgs, ft2, time);
            break;
        */
        default:
            std::cerr << "comparison method not implemented\n";
    }
}

void funcTrace::postDominatorContent(Comparison &c, int &diff, map<string, cfg_t *> &cfgs, funcTrace *ft2, long& time)
{
    unsigned i = 0, j = 0;
    vector<tuple<unsigned, unsigned>> alignedPairs;
    c.postDominatorApproach(alignedPairs, diff, c.loopPath1, c.loopPath2);
    for(unsigned t = 1; t < alignedPairs.size() - 1; t++) 
    {
        i = get<0>(alignedPairs[t]) - 1;
        j = get<1>(alignedPairs[t]) - 1;
        //cout << "doing ftcmp for i: " << i << ", j: " << j << endl;
        this->__funcTrace[i]->ftcmp(ft2->getFuncTrace()[j], cfgs, time, diff);
    }
} 

void funcTrace::greedyContent(Comparison &c, int &diff, map<string, cfg_t *> &cfgs, funcTrace *ft2, long &time)
{    
    unsigned i = 0; unsigned j = 0;
    vector<tuple<unsigned, unsigned>> alignedPairs;
    c.greedyApproach(alignedPairs, diff);
    for(unsigned t = 1; t < alignedPairs.size() - 1; t++)
    {
        i = get<0>(alignedPairs[t]) - 1;
        j = get<1>(alignedPairs[t]) - 1;
        //cout << "doing ftcmp for i: " << i << ", j: " << j << endl;
        this->__funcTrace[i]->ftcmp(ft2->getFuncTrace()[j], cfgs, time, diff);
    }
}

void funcTrace::editDistanceContent(Comparison &c, int &diff, map<string, cfg_t *> &cfgs, funcTrace *ft2, long &time)
{
    
    vector<node *> path1 = c.hpath;
    vector<node *> path2 = c.vpath;

    unsigned i = 0; unsigned j = 0;
    vector<std::tuple<unsigned, unsigned>> alignedPairs;
    c.editDistance(alignedPairs);
    unsigned prevI = 0; unsigned prevJ = 0;
    for(unsigned t = 0; t < alignedPairs.size(); t++)
    {
        i = std::get<0>(alignedPairs[t]);
        j = std::get<1>(alignedPairs[t]);
        if((i - prevI != 1 || j - prevJ != 1) && (i != 0 || j != 0))
        {
            cout << "diverted at "; 
            path1[prevI]->printNodeSimply(cout);
            cout << ", (i, j) = (" << prevI << ", " << prevJ << ")\nconverged at ";
            path1[i]->printNodeSimply(cout);
            cout << ", (i, j) = (" << i << ", " << j << ")\n";
            diff++;
        }

        /*
        path1[i]->printNodeSimply(cout);
        cout << " vs. ";
        path2[j]->printNodeSimply(cout);
        cout << " i: " << i << ", j: " << j << endl;
        */

        assert(path1[i] == path2[j]);

        if(path1[i]->type == nodeType::funcCall)
        {
            this->__funcTrace[i - 1]->ftcmp(ft2->getFuncTrace()[j - 1], cfgs, time, diff);
        }

        prevI = i; prevJ = j;
    }
}

void funcTrace::loopGreedyContent(Comparison &c, int &diff, map<string, cfg_t *> &cfgs, funcTrace *ft2, long &time)
{
    unsigned i = 0; unsigned j = 0;
    vector<std::tuple<unsigned, unsigned>> alignedPairs;
    c.loopGreedyApproach(c.loopPath1, c.loopPath2, alignedPairs, diff);
    for(unsigned t = 1; t < alignedPairs.size() - 1; t++)
    {
        i = std::get<0>(alignedPairs[t]) - 1;        
        j = std::get<1>(alignedPairs[t]) - 1; 
        //cout << "going to work on (" << i << ", " << j << ")\n";
        this->__funcTrace[i]->ftcmp(ft2->getFuncTrace()[j], cfgs, time, diff);
    }
}

/*
void funcTrace::regExContent(Comparison &c, int &diff, map<string, cfg_t *> &cfgs, funcTrace *ft2, long &time) 
{
    unsigned i, j;
    vector<tuple<unsigned, unsigned>> alignedPairs;
    c.regEx(alignedPairs, diff);
    cout << "printing alignedPairs";
    for(unsigned i = 0; i < alignedPairs.size(); i++)
    {
        cout << alignedPairs[i] << ", " << endl;
    }
    
    for(unsigned t = 1; t < alignedPairs.size() - 1; t++)
    {
        i = std::get<0>(alignedPairs[t]) - 1;        
        j = std::get<1>(alignedPairs[t]) - 1; 
        cout << "going to work on (" << i << ", " << j << "), "
            << __funcTrace[i]->funcName << ", " << ft2->getFuncTrace()[j]->funcName << endl;
        this->__funcTrace[i]->ftcmp(ft2->getFuncTrace()[j], cfgs, time, diff);
    }
  
}
*/

int funcTrace::getAPathRecursive(map<string, cfg_t *> cfgs)
{
    if(this->__funcTrace.size() == 0)
    {
        return 1; // this is to not to return 0 and make the number of paths 0
    }

    cfg_t *cfg = cfgs[this->funcName];

    //cout << this->funcName << endl;
    vector<node *> path = getPossiblePath(this, cfg);
    //cout << "printing path" << endl;
    //printPath(path);

    for(unsigned i = 0; i < this->__funcTrace.size(); i++)
    {
        this->__funcTrace[i]->getAPathRecursive(cfgs);
    }
    return 0;
}

long funcTrace::cntPaths(map<string, cfg_t *> cfgs)
{
    if(this->__funcTrace.size() == 0)
    {
        return 1; // this is to not to return 0 and make the number of paths 0
    }

    cfg_t *cfg = cfgs[this->funcName];

    cout << this->funcName << endl;
    vector<vector<node *>> paths = getAllPossiblePaths(this, cfg);

    long numPaths = paths.size();
    if(numPaths == 0)
    {
        cout << "found 0 paths at: " << this->funcName << '\n';
    }

    cout << this->funcName << ", numPaths: " << numPaths << "\n";
    for(unsigned i = 0; i < this->__funcTrace.size(); i++)
    {
        numPaths *= this->__funcTrace[i]->cntPaths(cfgs);
    }
    cout << this->funcName << ", totalPaths: " << numPaths << "\n";
    return numPaths;
}

/* 
 * moves index forward until the part where the return is matched to the function
 * returns 0 for success, non-0 for failure
 */
int funcTrace::matchingFuncs(const vector<string> &flatTrace, unsigned &index)
{
    vector<string> callStack;
    string currStr;
    while(true)
    {
        currStr = flatTrace[index++];
        if(currStr[0] == '/')
        {
            currStr.erase(0, 1);
            if(callStack.size() == 0)
            {
                fprintf(stderr, "could not find the matching function %s\n", currStr.c_str());
                return 1;
            }

            while(callStack.back() != currStr)
            {
                // keep on popping until finding the actual match
                callStack.pop_back();
                if(callStack.size() == 0)
                {
                    fprintf(stderr, "could not find the matching function %s\n", currStr.c_str());
                    return 1;
                }
                 
            }
            // popping one last time
            assert(callStack.back() == currStr);
            callStack.pop_back();
            if(callStack.size() == 0)
            {
                // returning successfully
                return 0;
            }
        } 
        else // when this is not return
        {
            // just shove it into the callStack
            callStack.push_back(currStr);
        }
    }
    
}

/* 
 * WHAT THE FUNCTION DOES 
 * 1. gets all the function calls from the cfgs
 * 2. retrieves the current function
 * 3. makes sure if that the function belongs to the libc.a
 * 4. sees if any of them belongs to the function that does not return
 * 5. gets the return statement right out (because it is already in libc.a)
 * 
 * THE PROBLEM WITH THIS FUNCTION 
 * we have this sequence: getopt_long, __getopt_long, getopt, /getopt, /__getopt_long(, /getopt_long)
 * so what we need to do is do this matching until the /__getopt_long
 * we can assert that __getopt_long is not nonRet function
 */
unsigned funcTrace::copeNoRetFuncs(unsigned &index, const vector<string> &flatTrace, const map<string, int> &clibDict, map<string, cfg_t *> &cfgs, map<string, int>& noRetFuncs)
{
   // read all the functions in the current function 
   string originalFunc = flatTrace[index];
   map<string, int> funcs = cfg2funcs(cfgs, flatTrace[index]); 
   //cout << "map" << endl;
   //Tools::print_map(funcs);
   string currStr = flatTrace[++index];

   /* this part of the algorithm is problematic because you are not aware whether or not the function is returning */
   while(funcs.find(currStr) != funcs.end()) // as long as you find that funcs you continue 
   {

        // check if this is not in noRetFuncs
        bool next = false;
        if(noRetFuncs.find(currStr) != noRetFuncs.end())
        {
            //cout << currStr << " is a noRetFunc inside noRetFunc\n";
            index = copeNoRetFuncs(index, flatTrace, clibDict, cfgs, noRetFuncs);
            //cout << "came back from " << currStr << endl;
            currStr = flatTrace[++index];
            next = true;
        }

        if(matchingFuncs(flatTrace, index))
        {
            fprintf(stderr, "matching funcs did not work correctly\n");
            if(next)
            {
                cout << "I am going to interpret this as an end\n";
                break;
            }
            else
            {
                exit(1);
            }
        }       
    
        // here trying to get the next call 
        currStr = flatTrace[index];
        if(currStr[0] == '/')
        {
            string tmp = currStr;
            tmp.erase(0, 1);
            if(funcs.find(tmp) != funcs.end())
            {
                fprintf(stderr, "this is not the expected case: %s\n", tmp.c_str());
                exit(1);
            }
        }
   }
   index--;
   return index;
}

funcTrace *funcTrace::makeFuncTrace(vector<string>& flatTrace, map<string, int>& clibDict, map<string, int>& noRetFuncs, map<string, cfg_t *>& cfgs)
{
    vector<funcTrace *> callStack {};
    string currStr;
    string tmpStr;
    //funcTrace *rv;
    funcTrace *currFT;
    funcTrace *currEle;
    //unsigned cnt = 0;

    for(unsigned index = 0; index < flatTrace.size(); index++) {
        // determine if this is a return or not
        currStr = flatTrace[index];
        //cout << "currStr: " << currStr << endl;
       
        if(currStr[currStr.length() - 1] == '\n') {
            currStr.resize(currStr.size() - 1);
        }

        if(currStr[0] == '/') {
            currStr.erase(0, 1);
            // it needs to put an end to the current fTrace and go up one more
            if(currFT->getFuncName() != currStr) {
                cerr << "it is not returning to the right function\n"   
                    << "index: " << index << endl
                    << "currFT->getFuncName(): " << currFT->getFuncName() << endl
                    << currStr << endl;
                exit(1);
            }

            if(currStr != "main") {
                currFT = callStack.back();
                assert(currFT != nullptr);
                callStack.pop_back();
            }
        } else {
            // it needs to add the funcElement to the trace
            // then it needs to go into another trace
            if(currStr == "main") {
                //rv = new funcTrace(currStr); 
                this->funcName = currStr;
                //callStack.push_back(rv);
                currFT = this;
            } else {
                // create a function trace element
                currEle = new funcTrace(currStr);
                currFT->push_back(currEle);

                if(noRetFuncs.find(currStr) != noRetFuncs.end()) {
                    //cout << currStr << " is noRetFunc!\n";
                    index = copeNoRetFuncs(index, flatTrace, clibDict, cfgs, noRetFuncs);
                    //cout << "came back from " << currStr << endl;
                } else {
                    if(clibDict.find(currStr) == clibDict.end()) {
                        // if the currStr was not found in clibDict
                        callStack.push_back(currFT);
                        currFT = currEle;
                    } else {
                        // if found in the currStr
                        // assert if next string is '/' + currStr
                        index++;
                        assert(flatTrace.size() > index);
                        /*
                        if(flatTrace.size() <= index) 
                        {
                            fprintf(stderr, "flatTrace.size(): %u <= index: %u\ncurrStr: %s\n", flatTrace.size(), index, currStr.c_str());
                            exit(1);
                        }
                        */
                        tmpStr = flatTrace[index]; 
                        if(tmpStr[0] != '/') {
                            cerr << "assertion of tmpStr[0] == \'/\' failed\nprint tmpStr: "
                                << tmpStr << endl << "exitting...\n";
                            exit(1);
                        }
                        tmpStr.erase(0, 1);
                        assert(tmpStr == currStr);
                    }
                }
            }

        }
    }
    if(callStack.size() != 0) {
        cout << callStack.size() << endl;
        cout << (unsigned)(-1) << endl;
        exit(1);
    }
    return this;
}

map<string, int> funcTrace::cfg2funcs(map<string, cfg_t *> &cfgs, string currFunc)
{
    map<string, int> funcs;
    //cout << currFunc << endl;
    cfg_t *currCFG = cfgs[currFunc];

    if(currCFG == nullptr)
    {
        fprintf(stderr, "could not find cfg for in cfg2funcs: %s\n", currFunc.c_str());
        exit(1);
    }

    map<node *, int> visited;
    node *currNode = currCFG->ep;
    __dfsFind(currNode, visited, funcs);
    //cout << "print map at the end" << endl;
    //print_map(funcs);

    return funcs;
}

void funcTrace::__dfsFind(node *currNode, map<node *, int> &visited, map<string, int> &funcs)
{
    visited[currNode] = 1; // currNode is in visited now;
    if(currNode->type == nodeType::funcCall)
    {
        //cout << "adding " << ((funcNode *)currNode)->funcName << endl;
        funcs[((funcNode *)currNode)->funcName] = 1; 
    }

    node *tmp;
    for(int i = 0; i < currNode->numOEdges; i++)
    {
        tmp = currNode->oEdges[i].ptr;
        if(visited.find(tmp) == visited.end())
        {
            __dfsFind(tmp, visited, funcs);
        }
    }
} 

void funcTrace::print_currFT(char const *const filename)
{
    FILE *fp = fopen(filename, "w");
    vector<funcTrace *>ft = this->__funcTrace;

    fwrite(this->funcName.c_str(), 1, strlen(this->funcName.c_str()), fp);
    fwrite(":\n\n", 1, strlen(":\n\n"), fp);

    for(unsigned i = 0; i < __funcTrace.size(); i++)
    {
        fwrite(__funcTrace[i]->funcName.c_str(), 1, strlen(__funcTrace[i]->funcName.c_str()), fp);
        fwrite("\n", 1, 1, fp); 
    }

    fclose(fp);
}
