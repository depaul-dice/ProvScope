// Copyright (c) 2021 Yuta Nakamura. All rights reserved.
#ifndef FUNCTRACE_H
#define FUNCTRACE_H

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <openssl/sha.h>
#include <chrono>

#include "ec.h"
#include "cfg.h"
#include "tools.h"
#include "Comparison.h"

#define FILEREADSIZE 4096
//#define PRINTPOSSIBLEPATH 0

#define LOOPGREEDY 0
#define REGEX 1
#define GREEDY 2
#define EDITDISTANCE 3
#define POSTDOMINATOR 4

class funcTrace 
{
public:
    /* big 4 */
    funcTrace() = default;
    funcTrace(const funcTrace&) = default;
    funcTrace& operator = (const funcTrace&) = default;
    ~funcTrace();

    funcTrace(std::string funcName);

    // these are operators basically
    void push_back(funcTrace*); // when successful returns 0, not successful, non-0
    bool deepEqual(const funcTrace&);
    bool operator == (const funcTrace&); // this just looks at surfaceHV
    funcTrace* operator[] (std::size_t idx);
    const funcTrace* operator[] (std::size_t idx) const;

    // retrieving members
    std::string getFuncName(void);
    std::vector<funcTrace *> getFuncTrace(void);

    unsigned char *updateHash();

    friend std::ostream& operator << (std::ostream& os, funcTrace const&);
    friend std::ostream& operator << (std::ostream& os, std::vector<funcTrace *> const&);
    void printRecursiveFT(std::ostream& os);

    // comparing two function trace
    void ftcmp(funcTrace *, std::map<std::string, cfg_t *>&, long&, int&);
    // count the number of paths by getting all the paths, the algorithm is faulty
    long cntPaths(std::map<std::string, cfg_t *> cfgs);
    // get one path that does not contradict the function trace
    int getAPathRecursive(std::map<std::string, cfg_t *> cfgs);
    // creating hierarchy on the function name 
    funcTrace *makeFuncTrace(std::vector<std::string>&ft, std::map<std::string, int>& clibDict, std::map<std::string, int>& noRetFuncs, std::map<std::string, cfg_t *>& cfgs);

    void print_currFT(char const *const filename); 

    // creation of hierarchical function trace should also be here
private:
    std::string funcName;
    std::vector<funcTrace *> __funcTrace;
    std::vector<virtualNode *> virtualNodes;
    //SHA_CTX deepHV; 
    //SHA_CTX surfaceHV;
    unsigned char surfaceDigest [20];
    unsigned char deepDigest [20];
    int DEBUG;

    //friend class funcTraceElement;
        
    // helper function for the makeFuncTrace when it faces noRetFuncs
    unsigned copeNoRetFuncs(unsigned &index, const std::vector<std::string> &flatTrace, const std::map<std::string, int> &clibDict, std::map<std::string, cfg_t *> &cfgs, std::map<std::string, int> &noRetFuncs);

    // helper function for to get the functions to get cfgs
    std::map<std::string, int> cfg2funcs(std::map<std::string, cfg_t *> &cfgs, std::string currFunc);

    // helper function for cfgs2funcs, just does the dfs and find the functions in the graphs
    void __dfsFind(node *currNode, std::map<node *, int> &visited, std::map<std::string, int> &funcs);

    // helper function for copeNoRetFuncs, moves the index to where the first function returns 
    int matchingFuncs(const std::vector<std::string> &flatTrace, unsigned &index);

    void print_digest(unsigned char *digest);

    // these reference and copy are by design
    void __printRecursiveFT(std::ostream& os, int tabs, int &count);
    void printPath(std::ostream& os, std::vector<node *> path);
    void printLooppath(std::ostream& os, std::vector<std::tuple<node *, unsigned>>& loopPath, unsigned numTab);
    std::vector<std::tuple<node *, unsigned>> cognizeLoops(std::vector<node *>& originalPath);

    void __ftcmp(int which, Comparison &c, std::map<std::string, cfg_t *>& cfgs, funcTrace *ft2, int &diff, long &time);
    void greedyContent(Comparison &c, int &diff, std::map<std::string, cfg_t *> &cfgs, funcTrace *ft2, long &time);
    void editDistanceContent(Comparison &c, int &diff, std::map<std::string, cfg_t *> &cfgs, funcTrace *ft2, long &time);
    void loopGreedyContent(Comparison &c, int &diff, std::map<std::string, cfg_t *> &cfgs, funcTrace *ft2, long &time);
    void postDominatorContent(Comparison &c, int &diff, std::map<std::string, cfg_t *> &cfgs, funcTrace *ft2, long &time);
    //void regExContent(Comparison &c, int &diff, std::map<std::string, cfg_t *> &cfgs, funcTrace *ft2, long &time);
};

#endif /* FUNCTRACE_H */
