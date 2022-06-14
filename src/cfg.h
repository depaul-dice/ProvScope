// Copyright (c) 2021 Yuta Nakamura. All rights reserved.
#ifndef CFG_H
#define CFG_H
#include <vector>
#include <iostream>
#include "ec.h"

#define NORMAL 0
#define BACKEDGE 1

enum class nodeType
{
    ePoint,
    sysCall,
    funcCall,
    empty,
    retCall,
    Virtual, // made into capital to distinguish from the keyword in cpp
};

std::ostream& operator << (std::ostream& os, const nodeType&);

class node;
class cfg_t;

struct edge 
{
    edge() =  delete;
    edge(const edge&) = delete;
    edge& operator = (const edge&) = delete;
    ~edge();

    edge(node *ptr);
    edge(node *ptr, int type);
    edge(char *name);
    edge(char *name, int type);
    
    union
    {
        node *ptr;
        char *name;
    };
    int type;
};

class node
{
public:
    // big 4
    node() = delete;
    node(const node&);
    node& operator = (const node&);
    ~node();

    node(nodeType nt, char *_name);
    node(char const * const name, const nodeType& nt, const int& numOEdges, const int &numIEdges, char **outNames, char **inNames, cfg_t *cfg); 
    node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, edge* const& _oEdges, edge * const& _iEdges, cfg_t *cfg);

    node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, const int& numBEdges, char **outNames, char **inNames, char **backNames, cfg_t *cfg);
    node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, const int& numBEdges, edge* const& _oEdges, edge* const& _iEdges, edge* const& _bEdges, cfg_t *cfg);

    friend std::ostream& operator << (std::ostream& os, const node&);
    void printNodeSimply(std::ostream& os);

    // members
    char *name;
    nodeType type;
    int color;
    int numOEdges;
    int numIEdges;
    struct edge *oEdges;
    struct edge *iEdges;
    cfg_t *cfg;
    bool loop;
};

class virtualNode: public node
{
public:
    virtualNode() = delete;
    virtualNode(const virtualNode&) = delete;
    virtualNode& operator = (const virtualNode&) = delete;
    ~virtualNode();
    
    virtualNode(std::vector<std::tuple<node *, unsigned>>& cs, char *_name);

    std::vector<std::tuple<node *, unsigned>> contents;
};

class funcNode: public node
{
public:
    // big 4
    funcNode() = delete;
    funcNode(const funcNode&);
    funcNode& operator = (const funcNode&) = delete;
    ~funcNode();

    funcNode(char const * const funcName, char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, char **outNames, char **inNames, cfg_t *cfg);
    funcNode(char const * const funcName, char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, const int& numBEdges, char **outNames, char **inNames, char **backNames, cfg_t *cfg);

    // members
    char *funcName;
};

class sysNode: public node
{
public:
    // big 4
    sysNode() = delete;
    sysNode(const sysNode&);
    sysNode& operator = (const sysNode&) = delete;
    ~sysNode();

    sysNode(int sysNum, char const * const name, nodeType nt, int numOEdges, int numIEdges, char **outNames, char **inNames, cfg_t *cfg);
    sysNode(int sysNum, char const * const name, nodeType nt, int numOEdges, int numIEdges, int numBEdges, char **outNames, char **inNames, char **backEdges, cfg_t *cfg);

    // members
    int sysNum;
};

class cfg_t 
{
public:
    // big 4
    cfg_t() = delete;
    cfg_t(const cfg_t&) = default; // too much of a hassle, I dont wanna deal with it now
    cfg_t& operator = (const cfg_t&) = default; // too much of a hassle, I dont wanna deal with it now
    ~cfg_t(); 

    cfg_t(char *pathname);
    friend std::ostream& operator << (std::ostream& os, const cfg_t& cfg);
    void clearTraverse();

    // members
    char *name;
    node *ep; //entry point
    unsigned int numNodes;
};

#endif /* CFG_H */
