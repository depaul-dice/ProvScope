// Copyright (c) 2021 Yuta Nakamura. All rights reserved

#include "cfg.h"
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <typeinfo>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cassert>

#define WHITE 0
#define BLACK 1
#define ONELINE 4096 

using namespace std;

enum class NodeInfos
{
    NAME,
    TYPE,
    NUMOUTS,
    OUTS,
    NUMINS,
    INS,
    NUMBACKS,
    BACKS,
    FUNCNAME,
    SYSNUM,
    OTHER
};

std::ostream& operator << (std::ostream& os, const nodeType& tmp)
{
    if(tmp == nodeType::ePoint)
        os << "ePoint";
    else if(tmp == nodeType::sysCall)
        os << "sysCall";
    else if(tmp == nodeType::funcCall)
        os << "funcCall";
    else if(tmp == nodeType::empty)
        os << "empty";
    else if(tmp == nodeType::retCall)
        os << "retCall";
    else if(tmp == nodeType::Virtual)
        os << "virtual";
    else 
        os << "unKnown";
    return os;
}

edge::~edge()
{
}

edge::edge(node *ptr)
    :ptr(ptr), type(NORMAL)
{
}

edge::edge(node *ptr, int type)
    :ptr(ptr), type(type)
{
}

edge::edge(char *name)
    :name(name), type(NORMAL)
{
}

edge::edge(char *name, int type)
    :name(name), type(type)
{
}

node::node(const node& tmp)
    :type(tmp.type), color(tmp.color), numOEdges(tmp.numOEdges), numIEdges(tmp.numIEdges), cfg(tmp.cfg), loop(tmp.loop)
{
    oEdges = (edge *)malloc(sizeof(edge) * numOEdges);
    for(int i = 0; i < numOEdges; i++)
    {
        size_t len = strlen(tmp.oEdges[i].name) + 1;
        oEdges[i].name = (char *)malloc(len);
        memcpy(oEdges[i].name, tmp.oEdges[i].name, len); 
        oEdges[i].type = NORMAL;
    }
    iEdges = (edge *)malloc(sizeof(edge) * numIEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        size_t len = strlen(tmp.iEdges[i].name) + 1;
        iEdges[i].name = (char *)malloc(len);
        memcpy(iEdges[i].name, tmp.iEdges[i].name, len);
        iEdges[i].type = NORMAL;
    }
   
}

node& node::operator = (const node& tmp)
{
    type = tmp.type;
    color = tmp.color;
    numOEdges = tmp.numOEdges;
    numIEdges = tmp.numIEdges;
    cfg = tmp.cfg;
    loop = tmp.loop;

    for(int i = 0; i < numOEdges; i++)
    {
        free(oEdges[i].name);
    }
    free(oEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        free(iEdges[i].name);
    }
    free(iEdges);

    oEdges = (edge *)malloc(sizeof(edge) * numOEdges);
    size_t len = 0; 
    for(int i = 0; i < numOEdges; i++)
    {
        len = strlen(tmp.oEdges[i].name) + 1;
        oEdges[i].name = (char *)malloc(len);
        memcpy(oEdges[i].name, tmp.oEdges[i].name, len); 
        oEdges[i].type = NORMAL;
    }

    iEdges = (edge *)malloc(sizeof(edge) * numIEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        len = strlen(tmp.iEdges[i].name) + 1;
        iEdges[i].name = (char *)malloc(len);
        memcpy(iEdges[i].name, tmp.iEdges[i].name, len);  
        iEdges[i].type = NORMAL;
    }
    
    return *this;    
}

node::~node()
{
    if(type == nodeType::funcCall)
    {
        funcNode *tmp = (funcNode *)this;
        free(tmp->funcName);
    }
    if(name != nullptr)
        free(name);
    if(oEdges != nullptr)
        free(oEdges);
    if(iEdges != nullptr)
        free(iEdges);
}

node::node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, char **outNames, char **inNames, cfg_t *cfg)
    :name(nullptr), type(nt), color(WHITE), numOEdges(numOEdges), numIEdges(numIEdges), cfg(cfg), loop(false)
{
    size_t len = strlen(name) + 1;
    this->name = (char *)malloc(len);
    memcpy(this->name, name, len);

    oEdges = (edge *)malloc(sizeof(edge) * numOEdges);
    for(int i = 0; i < numOEdges; i++)
    {
        len = strlen(outNames[i]) + 1;
        oEdges[i].name = (char *)malloc(len);
        memcpy(oEdges[i].name, outNames[i], len); 
        oEdges[i].type = NORMAL;
    }

    iEdges = (edge *)malloc(sizeof(edge) * numIEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        if(inNames == nullptr)
        {
            std::cout << numIEdges << " " << name << "\n";
        }
        len = strlen(inNames[i]) + 1;
        
        iEdges[i].name = (char *)malloc(len);
        memcpy(iEdges[i].name, inNames[i], len);
        iEdges[i].type = NORMAL;
    }
}

node::node(nodeType nt, char *_name)
    : name(nullptr), type(nt), color(WHITE), numOEdges(0), numIEdges(0), oEdges(nullptr), iEdges(nullptr), cfg(nullptr), loop(false)
{
    size_t len = strlen(_name) + 1;
    name = (char *)malloc(len);
    memcpy(name, _name, len);
}

node::node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, const int& numBEdges, char **outNames, char **inNames, char **backNames, cfg_t *cfg)
    :name(nullptr), type(nt), color(WHITE), numOEdges(numOEdges + numBEdges), numIEdges(numIEdges), cfg(cfg), loop(false)
{
    size_t len = strlen(name) + 1;
    this->name = (char *)malloc(len);
    memcpy(this->name, name, len);

    oEdges = (edge *)malloc(sizeof(edge) * (numOEdges + numBEdges));
    int k;
    for(k = 0; k < numOEdges; k++)
    {
        len = strlen(outNames[k]) + 1;
        oEdges[k].name = (char *)malloc(len);
        memcpy(oEdges[k].name, outNames[k], len); 
        oEdges[k].type = NORMAL;
    }
    for(; k < numOEdges + numBEdges; k++)
    {
        len = strlen(backNames[k - numOEdges]) + 1;
        oEdges[k].name = (char *)malloc(len);
        memcpy(oEdges[k].name, backNames[k - numOEdges], len);
        oEdges[k].type = BACKEDGE;
    }

    iEdges = (edge *)malloc(sizeof(edge) * numIEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        if(inNames == nullptr)
        {
            std::cout << numIEdges << " " << name << "\n";
        }
        len = strlen(inNames[i]) + 1;
        
        iEdges[i].name = (char *)malloc(len);
        memcpy(iEdges[i].name, inNames[i], len);
        iEdges[i].type = NORMAL;
    }
}


node::node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, edge* const& _oEdges, edge* const& _iEdges, cfg_t *cfg)
    :name(nullptr), type(nt), color(WHITE), numOEdges(numOEdges), cfg(cfg), loop(false)
{
    fprintf(stderr, "is this ever used\n");
    assert(false);

    size_t len = strlen(name) + 1;
    this->name = (char *)malloc(len);
    memcpy(this->name, name, len);

    oEdges = (edge *)malloc(sizeof(edge) * numOEdges);
    for(int i = 0; i < numOEdges; i++)
    {
        len = strlen(_oEdges[i].name) + 1;
        oEdges[i].name = (char *)malloc(len);
        memcpy(oEdges[i].name, _oEdges[i].name, len); 
        oEdges[i].type = _oEdges[i].type;
    }

    iEdges = (edge *)malloc(sizeof(edge) * numIEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        len = strlen(_iEdges[i].name) + 1;
        iEdges[i].name = (char *)malloc(len);
        memcpy(iEdges[i].name, _iEdges[i].name, len); 
        iEdges[i].type = NORMAL;
    }

}

node::node(char const * const name, const nodeType& nt, const int& numOEdges, const int& numIEdges, const int& numBEdges, edge* const& _oEdges, edge* const& _iEdges, edge* const& _bEdges, cfg_t *cfg)
    :name(nullptr), type(nt), color(WHITE), numOEdges(numOEdges), cfg(cfg), loop(false)
{
    fprintf(stderr, "I dont think this has ever been used. has it?\n");
    assert(false);

    size_t len = strlen(name) + 1;
    this->name = (char *)malloc(len);
    memcpy(this->name, name, len);

    oEdges = (edge *)malloc(sizeof(edge) * (numOEdges + numBEdges));
    for(int i = 0; i < numOEdges; i++)
    {
        // this block seems wrong to me
        len = strlen(_oEdges[i].name) + 1;
        oEdges[i].name = (char *)malloc(len);
        memcpy(oEdges[i].name, _oEdges[i].name, len); 
        oEdges[i].type = NORMAL;

    }

    for(int i = numOEdges; i < numOEdges + numBEdges; i++)
    {
        len = strlen(_bEdges[i].name) + 1;
        // you need to add shit here if necessary\n
    }

    iEdges = (edge *)malloc(sizeof(edge) * numIEdges);
    for(int i = 0; i < numIEdges; i++)
    {
        len = strlen(_iEdges[i].name) + 1;
        iEdges[i].name = (char *)malloc(len);
        memcpy(iEdges[i].name, _iEdges[i].name, len); 
        iEdges[i].type = NORMAL;
    }

}

std::ostream& operator << (std::ostream& os, const node& tmp)
{
    os << tmp.name << ",";
    os << tmp.type << ",";
    if(tmp.type == nodeType::sysCall)
    {
        sysNode *_tmp = (sysNode *)&tmp;
        os << _tmp->sysNum << ",";
    }
    else if(tmp.type == nodeType::funcCall)
    {
        funcNode *_tmp = (funcNode *)&tmp;
        os << _tmp->funcName << ",";
    }

    if(tmp.loop == true)
        os << "loop,";

    /*
    os << tmp.numOEdges << ",";
    os << tmp.numIEdges << ",";
    node *curr = nullptr;

    int i = 0;

    for(; i < tmp.numOEdges - 1; i++)
    {
        if(tmp.oEdges[i].type == BACKEDGE)
            os << "_";
        curr = tmp.oEdges[i].ptr; 
        os << curr->name << ":";
    } 
    
    if(tmp.numOEdges >= 1)
    {
        if(tmp.oEdges[i].type == BACKEDGE)
            os << "_";
        curr = tmp.oEdges[i].ptr;
        os << curr->name;
    } 
    else
    {
        os << "na";
    }
    
    os << ",";

    for(i = 0; i < tmp.numIEdges - 1; i++)
    {
        curr = tmp.iEdges[i].ptr;
        os << curr->name << ":";
    }

    if(tmp.numIEdges >= 1)
    {
        curr = tmp.iEdges[i].ptr;
        os << curr->name;
    }
    else
    {
        os << "na";
    }
    */
    
    if(tmp.cfg != nullptr)
    {
        os << ',' << tmp.cfg->name;
    }
    
    return os;
}

void node::printNodeSimply(std::ostream& os)
{
    os << this->name << ",";
    os << this->type << ",";
    if(this->type == nodeType::sysCall)
    {
        sysNode *_tmp = (sysNode *)this;
        os << _tmp->sysNum << ",";
    }
    else if(this->type == nodeType::funcCall)
    {
        funcNode *_tmp = (funcNode *)this;
        os << _tmp->funcName << ",";
    }

    if(this->loop == true)
        os << "loop,";

    os << this->numOEdges << "," << this->numIEdges;
    if(this->cfg != nullptr)
    {
        os << ',' << this->cfg->name;
    }

}

bool node::operator == (const node& rhs) const
{
    return strcmp(this->name, rhs.name) == 0;
}

bool node::operator == (const virtualNode &rhs) const
{
    return false;
}

virtualNode::virtualNode(const virtualNode &tmp)
    :node(nodeType::Virtual, tmp.name), contents(tmp.contents), head(nullptr)
{
    if(contents.size() > 0) {
        head = get<0>(contents[0]);
    } else {
        cerr << "could not find head: " << *this << endl;
    }
}

virtualNode& virtualNode::operator = (const virtualNode& tmp) 
{
    contents = tmp.contents;
    if(contents.size() > 0) {
        head = get<0>(tmp.contents[0]);
    } else {
        cerr << "could not find head: " << *this << endl;
    }
    return *this;
}

virtualNode::~virtualNode()
{
    /*
    node *curr;
    for(unsigned i = 0; i < contents.size(); i++)
    {
        curr = get<0>(contents[i]);
        if(curr->type == nodeType::Virtual)
        {
            delete curr;
        }
    }
    */
}

virtualNode::virtualNode(std::vector<std::tuple<node *, unsigned>> &cs, char *_name)
    : node(nodeType::Virtual, _name), contents(cs), head(nullptr)
{
    if(cs.size() > 0) {
        head = get<0>(cs[0]);
    } else {
        cerr << "could not find head: " << *this << endl;
    }
}

bool virtualNode::operator == (const node& rhs) const
{
    return false;
}

bool virtualNode::operator == (const virtualNode& rhs) const
{
    return *(this->head) == *(rhs.head);
}

funcNode::~funcNode()
{
    free(funcName);
    free(name);
    free(oEdges);
    free(iEdges);
}

funcNode::funcNode(const char *const funcName, char const * const name, const nodeType& nt, const int &numOEdges, const int& numIEdges, char **outNames, char **inNames, cfg_t *cfg)
    :node(name, nt, numOEdges, numIEdges, outNames, inNames, cfg)
{
    size_t len = strlen(funcName) + 1;
    this->funcName = (char *)malloc(len);
    memcpy(this->funcName, funcName, len);
}

funcNode::funcNode(const char *const funcName, char const * const name, const nodeType& nt, const int &numOEdges, const int& numIEdges, const int &numBEdges, char **outNames, char **inNames, char **backNames, cfg_t *cfg)
    :node(name, nt, numOEdges, numIEdges, numBEdges, outNames, inNames, backNames, cfg)
{
    size_t len = strlen(funcName) + 1;
    this->funcName = (char *)malloc(len);
    memcpy(this->funcName, funcName, len);
}


funcNode::funcNode(const funcNode& tmp)
    :node(tmp.name, tmp.type, tmp.numOEdges, tmp.numIEdges, tmp.oEdges, tmp.iEdges, tmp.cfg), funcName(tmp.funcName)
{
    size_t len = strlen(tmp.funcName) + 1;
    funcName = (char *)malloc(len);
    memcpy(funcName, tmp.funcName, len); 
}

/*
funcNode& funcNode::operator = (const funcNode& tmp)
{
    node = tmp.node;

    funcName(tmp.funcName);
    return *this;
}
*/

sysNode::~sysNode()
{
    free(name);
    free(oEdges);
    free(iEdges);
}

sysNode::sysNode(int sysNum, char const * const name, nodeType nt, int numOEdges, int numIEdges, char **outNames, char **inNames, cfg_t *cfg)
    :node(name, nt, numOEdges, numIEdges, outNames, inNames, cfg), sysNum(sysNum)
{ 
}

sysNode::sysNode(int sysNum, char const * const name, nodeType nt, int numOEdges, int numIEdges, int numBEdges, char **outNames, char **inNames, char **backNames, cfg_t *cfg)
    :node(name, nt, numOEdges, numIEdges, numBEdges, outNames, inNames, backNames, cfg), sysNum(sysNum)
{ 
}

sysNode::sysNode(const sysNode& tmp)
    :node(tmp.name, tmp.type, tmp.numOEdges, tmp.numIEdges, tmp.oEdges, tmp.iEdges, tmp.cfg), sysNum(tmp.sysNum)
{
}

/*
sysNode& sysNode::operator = (const sysNode& tmp)
{
    node = tmp.node;
    sysNum = tmp.sysNum;

    return *this;
}
*/

cfg_t::~cfg_t()
{
    std::queue<node *> Queue;
    std::stack<node *> s;
    Queue.push(ep);
    node *curr = nullptr;

    while(!Queue.empty())
    {
        curr = Queue.front();
        if(curr->color == WHITE)
        {
            s.push(curr);
            for(int i = 0; i < curr->numOEdges; i++)
            {
                Queue.push(curr->oEdges[i].ptr);
            }
        }        
        curr->color = BLACK;
        Queue.pop();
    }

    assert(numNodes == s.size());
    while(!s.empty())
    {
        curr = s.top();
        delete curr;
        s.pop();
    }
    if(this->name != nullptr)
        free(this->name);
}

static char **_parseEdges(char *ptr, int numEdges)
{
    if(ptr == nullptr || numEdges == 0)
    {
        assert(strcmp(ptr, "na") == 0);
        return nullptr;
    }

    char *line = (char *)malloc(ONELINE); 
    char **rv = (char **)malloc(sizeof(char *) * numEdges);

    strcpy(line, ptr);
    int j = 0;
    rv[j++] = line;
    for(int i = 0; i < ONELINE; i++)
    {
        if(line[i] == ':')
        {
            line[i] = '\0';
            rv[j++] = &(line[i + 1]);
        }
        else if(line[i] == '\0')
        {
            break;
        }
    }
    return rv; 
}

static node *_parseLine(char *line, cfg_t *cfg, ErrorCode &ec)
{
    char *ptr;
    NodeInfos flow = NodeInfos::NAME;
    nodeType nt;
    int numOEdges;
    char **outEdges = nullptr;
    int numIEdges;
    char **inEdges = nullptr;
    int numBEdges; 
    char **backEdges = nullptr;
    int sysNum;
    char id [PATH_MAX];
    char funcName [PATH_MAX];
    std::string line_cpy = line;
    
    for(ptr = strtok(line, ","); ptr != nullptr; ptr = strtok(nullptr, ",")) 
    {
        switch(flow) 
        {
            case NodeInfos::NAME:
                strcpy(id, ptr);
                flow = NodeInfos::TYPE;
                break;
            case NodeInfos::TYPE:
                if(strcmp("epoint", ptr) == 0) 
                {
                    nt = nodeType::ePoint; 
                }
                else if(strcmp("syscall", ptr) == 0)
                {
                    nt = nodeType::sysCall;
                }
                else if(strcmp("ret", ptr) == 0)
                {
                    nt = nodeType::retCall;
                }
                else if(strcmp("funccall", ptr) == 0)
                {
                    nt = nodeType::funcCall;
                }
                else if(strcmp("empty", ptr) == 0)
                {
                    nt = nodeType::empty;
                }
                else
                {
                    ec = ErrorCode::INVALID_TYPE; 
                }

                if(nt == nodeType::sysCall)
                {
                    flow = NodeInfos::SYSNUM;
                }
                else if(nt == nodeType::funcCall)
                {
                    flow = NodeInfos::FUNCNAME;
                }
                else
                {
                    flow = NodeInfos::NUMOUTS;
                }
                break;

            case NodeInfos::NUMOUTS:
                numOEdges = atoi(ptr);
                flow = NodeInfos::NUMINS;
                break;

            case NodeInfos::NUMINS:
                numIEdges = atoi(ptr);
                flow = NodeInfos::NUMBACKS; 
                break;

            case NodeInfos::NUMBACKS:
                numBEdges = atoi(ptr);
                flow = NodeInfos::OUTS;
                break;
                
            case NodeInfos::OUTS:
                outEdges = _parseEdges(ptr, numOEdges); 
                flow = NodeInfos::INS;
                break;

            case NodeInfos::INS: 
                inEdges = _parseEdges(ptr, numIEdges);
                flow = NodeInfos::BACKS;
                break;

            case NodeInfos::BACKS:
                backEdges = _parseEdges(ptr, numBEdges);
                flow = NodeInfos::OTHER;
                break;

            case NodeInfos::SYSNUM:
                sysNum = atoi(ptr);
                flow = NodeInfos::NUMOUTS;
                break;

            case NodeInfos::FUNCNAME:
                strcpy(funcName, ptr);
                flow = NodeInfos::NUMOUTS;
                break;

            default:
                fprintf(stderr, "wrong control flow %s\n", line);
                return nullptr;
        }
    }
    node *rv;
    if(nt == nodeType::ePoint || nt == nodeType::retCall || nt == nodeType::empty)
    {
        if(numBEdges > 0)
            rv = new node(id, nt, numOEdges, numIEdges, numBEdges, outEdges, inEdges, backEdges, cfg);
        else
            rv = new node(id, nt, numOEdges, numIEdges, outEdges, inEdges, cfg);
    }
    else if(nt == nodeType::funcCall)
    {
        if(numBEdges == 0)
        {
            rv = new funcNode(funcName, id, nt, numOEdges, numIEdges, outEdges, inEdges, cfg);
        }   
        else
        {    
            rv = new funcNode(funcName, id, nt, numOEdges, numIEdges, numBEdges, outEdges, inEdges, backEdges, cfg);
        }
    }
    else
    {
        assert(nt == nodeType::sysCall);
        if(numBEdges == 0)
        {
            rv = new sysNode(sysNum, id, nt, numOEdges, numIEdges, outEdges, inEdges, cfg); 
        }
        else
        {
            rv = new sysNode(sysNum, id, nt, numOEdges, numIEdges, numBEdges, outEdges, inEdges, backEdges, cfg); 
        }
    }

    if(outEdges == nullptr)
    {
        /*
        if(nt != nodeType::retCall && numBEdges <= 0)
        {
            std::cout << line_cpy << std::endl;

        }
        assert(nt == nodeType::retCall || numBEdges > 0);
        */
    }
    else
    {
        free(outEdges[0]);
        free(outEdges);
    }

    if(inEdges == nullptr)
    {
        assert(nt == nodeType::ePoint);
    }
    else
    {
        free(inEdges[0]);
        free(inEdges);
    }

    if(backEdges == nullptr)
    {
    }
    else
    {
        free(backEdges[0]);
        free(backEdges);
    }
    return rv;
}

static ErrorCode _getLine(char *lineptr, FILE *stream)
{
    static char line[ONELINE];
    char *ptr;
    unsigned len;
    if(!lineptr) 
    {
       return ErrorCode::INVALID_VALUE; 
    }
    if(ferror(stream)) 
    {
        return ErrorCode::INVALID_FPTR;
    }
    if(!fgets(line, ONELINE, stream))
    {
        if(fgetc(stream) == EOF)
            return ErrorCode::_EOF; 
        else
            return ErrorCode::READERROR; 
    } 

    ptr = strchr(line, '\n');
    if(ptr)
        *(ptr) = '\0';
    else
        return ErrorCode::NOLINEFEED;
    
    len = strlen(line) + 1;
    memcpy(lineptr, line, len);

    return ErrorCode::SUCCESS;
}

static char *extractSubstring(char *filename)
{
    size_t len = strlen(filename) - 1;
    for(int i = len; i >= 0; i--)
    {
        if(filename[i] == '/')
        {
            i++;
            return &filename[i];
        }
        else if(filename[i] == '.')
        {
            filename[i] = '\0';
        }
    }
    return filename;
}


cfg_t::cfg_t(char *pathname)
    :numNodes(0)
{
    
    FILE *fp = fopen(pathname, "r");
    char line[ONELINE]; 

    std::map<std::string, node *> nodes; 
    ErrorCode ec = ErrorCode::SUCCESS;
    node *Node = nullptr;
    while((ec = _getLine(line, fp)) == ErrorCode::SUCCESS)
    {
        Node = _parseLine(line, this, ec);
        assert(Node != nullptr);        
        if(Node->type == nodeType::ePoint)
        {
            this->ep = Node;
        }
        numNodes++;
        nodes[(std::string)Node->name] = Node;
        //nodes.insert(make_pair((std::string)Node->name, Node));
        //nodes[(std::string)Node->name] = *Node;
        /*
        if(Node->type == nodeType::funcCall)
        {
            fNode = (funcNode *)Node; 
        }
        else if(Node->type == nodeType::sysCall)
        {
            sNode = (sysNode *)Node;
        }
        */
    } 

    if(ec != ErrorCode::_EOF)
    {
        std::cerr << ec << std::endl;
        exit(1);
    }

    assert(this->ep != nullptr);

    /* one needs to match the char * with actual nodes here */
    std::map<std::string, node *>::iterator it;
    std::string ref;
    node *oedge;
    node *iedge;
    char *tmp;

    for(it = nodes.begin(); it != nodes.end(); it++)
    {
        Node = (it->second);
        for(int i = 0; i < Node->numOEdges; i++)
        {
            tmp = Node->oEdges[i].name;
            oedge = (nodes.find((std::string)tmp)->second); 
           
            Node->oEdges[i].ptr = oedge; 
            if(Node->oEdges[i].type == BACKEDGE)
            {
                oedge->loop = true;
            }
            free(tmp);
        }

        for(int i = 0; i < Node->numIEdges; i++)
        {
            tmp = Node->iEdges[i].name; 
            iedge = nodes[(std::string)tmp]; 
            Node->iEdges[i].ptr = iedge;
            free(tmp);
        }

    }
    
    fclose(fp);    
    tmp = extractSubstring(pathname);
    size_t len = strlen(tmp) + 1; 
    this->name = (char *)malloc(len);
    memcpy(this->name, tmp, len);
}

void cfg_t::clearTraverse()
{
    std::queue<node *>Queue;
    Queue.push(ep);
    node *curr = nullptr;
    while(!Queue.empty())
    {
        curr = Queue.front();
        if(curr->color == BLACK)
        {
            for(int i = 0; i < curr->numOEdges; i++)
            {
                Queue.push(curr->oEdges[i].ptr);
            }
        }
        curr->color = WHITE;
        Queue.pop();
    }
}

std::ostream& operator << (std::ostream& os, cfg_t const& cfg)
{
    std::queue<node *> Queue;   
    Queue.push(cfg.ep);
    node *curr = nullptr;
    while(!Queue.empty())
    {
        curr = Queue.front();
        if(curr->color == WHITE)
        {
            os << *(curr) << std::endl; 
            
            for(int i = 0; i < curr->numOEdges; i++)
            {
                Queue.push(curr->oEdges[i].ptr);
            }
        }
        curr->color = BLACK;
        Queue.pop();
    }
    os.flush();
    return os;
}

