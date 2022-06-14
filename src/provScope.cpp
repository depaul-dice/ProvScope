// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

//#include <dirent.h>
//#include <sys/stat.h>
#include <chrono>

#include "tools.h"
#include "readFiles.h"
#include "funcTrace.h"
#include "ec.h"
#include "cfg.h"
#include "Args.h"

// I want to write this function in some namespace or something later
funcTrace *initMainFT(void)
{
    funcTrace *ft = new funcTrace("main");
    return ft;
}

int pathCompare(Args& args)
{
    ErrorCode ec = ErrorCode::SUCCESS;
    std::cout << "reading cfgs\n";
    char *tmpStr = (char *)malloc(strlen(args.parsedDirectory.c_str()) + 1);
    strcpy(tmpStr, args.parsedDirectory.c_str());

    std::map<std::string, cfg_t *>cfgs = readCFGs(tmpStr, ec);
    free(tmpStr);
    //Tools::print_map(cfgs);

    std::cout << "making dict\n";
    std::map<std::string, int> clibDict = file2Dict(args.clibFile);    
    //Tools::print_map(clibDict);
    std::cout << "making flatTrace\n";
    std::vector<std::string> flatTrace1 = makeFlatTrace(args.flatTrace1);
    //print_vector(flatTrace);
    std::vector<std::string> flatTrace2 = makeFlatTrace(args.flatTrace2); 

    std::map<std::string, int> noRetFuncs = file2Dict(args.noRetFile);

    auto start1 = std::chrono::steady_clock::now();
    funcTrace *ft1 = new funcTrace();
    ft1->makeFuncTrace(flatTrace1, clibDict, noRetFuncs, cfgs);
    ft1->updateHash();
    auto end1 = std::chrono::steady_clock::now();

    auto start2 = std::chrono::steady_clock::now();
    funcTrace *ft2 = new funcTrace();
    ft2->makeFuncTrace(flatTrace2, clibDict, noRetFuncs, cfgs);
    ft2->updateHash();
    auto end2 = std::chrono::steady_clock::now();

    std::cout << "flatTrace1 functionTrace make time: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count()
        << " microseconds\n";

    std::cout << "flatTrace2 functionTrace make time: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count()
        << " microseconds\n";

    std::cout << "comparison has started" << std::endl;
    long time = 0;
    int diff = 0;
    auto tik = std::chrono::steady_clock::now();
    ft1->ftcmp(ft2, cfgs, time, diff);
    auto tok = std::chrono::steady_clock::now();
    std::cout << "comparison time: "
        << std::chrono::duration_cast<std::chrono::microseconds>(tok - tik).count()
        << " microseconds\n";
    std::cout << "path tracing time: "
        << time << " microseconds\n";
    
    std::cout << diff << " divergence were there\n";

    delete ft1; 
    delete ft2;
    std::map<std::string, cfg_t *>::iterator it;
    for(it = cfgs.begin(); it != cfgs.end(); it++)
    {
        delete it->second;
    }
    
    return EXIT_SUCCESS;
}

int findAllPath(Args& args)
{
    ErrorCode ec = ErrorCode::SUCCESS;

    char *tmpStr = (char *)malloc(strlen(args.parsedDirectory.c_str()) + 1);
    strcpy(tmpStr, args.parsedDirectory.c_str());
    std::map<std::string, cfg_t *>cfgs = readCFGs(tmpStr, ec);
    free(tmpStr);

    std::map<std::string, int> clibDict = file2Dict(args.clibFile);    
    std::vector<std::string> flatTrace1 = makeFlatTrace(args.flatTrace1);
    std::map<std::string, int> noRetFuncs = file2Dict(args.noRetFile);

    funcTrace *ft1 = new funcTrace();
    ft1 = ft1->makeFuncTrace(flatTrace1, clibDict, noRetFuncs, cfgs);

    ft1->cntPaths(cfgs);
    delete ft1; 
    
    std::map<std::string, cfg_t *>::iterator it;
    for(it = cfgs.begin(); it != cfgs.end(); it++)
    {
        delete it->second;
    }
 
    return EXIT_SUCCESS;
}

std::map<std::string, cfg_t *> getCFGs(Args &args)
{
    ErrorCode ec = ErrorCode::SUCCESS;
    char *tmpStr = (char *)malloc(strlen(args.parsedDirectory.c_str()) + 1);
    strcpy(tmpStr, args.parsedDirectory.c_str());
    std::map<std::string, cfg_t *>cfgs = readCFGs(tmpStr, ec);
    free(tmpStr);
    //Tools::print_map(cfgs);

    /*
    if (ec != ErrorCode::SUCCESS) 
    {
        std::cerr << ec << '\n';
    }
    */

    return cfgs; 
}
int findAPath(Args& args)
{
    /*
    ErrorCode ec = ErrorCode::SUCCESS;
    char *tmpStr = (char *)malloc(strlen(args.parsedDirectory.c_str()) + 1);
    strcpy(tmpStr, args.parsedDirectory.c_str());
    std::map<std::string, cfg_t *>cfgs = readCFGs(tmpStr, ec);
    free(tmpStr);
    */
    std::map<std::string, cfg_t *>cfgs = getCFGs(args);

    std::map<std::string, int> clibDict = file2Dict(args.clibFile);    
    std::vector<std::string> flatTrace1 = makeFlatTrace(args.flatTrace1);
    //print_vector(flatTrace1);
    std::map<std::string, int> noRetFuncs = file2Dict(args.noRetFile);

    std::cout << "making function trace hierarchical\n";
    funcTrace *ft1 = new funcTrace();
    ft1 = ft1->makeFuncTrace(flatTrace1, clibDict, noRetFuncs, cfgs);

    std::cout << "trying to find out the path\n";
    auto tik = std::chrono::steady_clock::now();
    ft1->getAPathRecursive(cfgs);
    auto tok = std::chrono::steady_clock::now();
    std::cout << "found a path\n";
    std::cout << "path finding time: "
        << std::chrono::duration_cast<std::chrono::microseconds>(tok - tik).count()
        << " microseconds\n";

    delete ft1; 
    
    std::map<std::string, cfg_t *>::iterator it;
    for(it = cfgs.begin(); it != cfgs.end(); it++)
    {
        delete it->second;
    }
 
    return EXIT_SUCCESS;
}

int printHierarchicalPath(Args& args)
{
    /*
    ErrorCode ec = ErrorCode::SUCCESS;
    char *tmpStr = (char *)malloc(strlen(args.parsedDirectory.c_str()) + 1);
    strcpy(tmpStr, args.parsedDirectory.c_str());
    std::map<std::string, cfg_t *>cfgs = readCFGs(tmpStr, ec);
    free(tmpStr);
    */
    std::map<std::string, cfg_t *>cfgs = getCFGs(args);

    std::map<std::string, int> clibDict = file2Dict(args.clibFile);    
    std::vector<std::string> flatTrace1 = makeFlatTrace(args.flatTrace1);
    std::map<std::string, int> noRetFuncs = file2Dict(args.noRetFile);

    std::cout << "making function trace hierarchical\n";
    funcTrace *ft1 = new funcTrace();
    ft1 = ft1->makeFuncTrace(flatTrace1, clibDict, noRetFuncs, cfgs);

    std::cout << "printing the path out\n"; 

    ft1->printRecursiveFT(std::cout);

    std::map<std::string, cfg_t *>::iterator it;
    for(it = cfgs.begin(); it != cfgs.end(); it++)
    {
        delete it->second;
    }
 
    return EXIT_SUCCESS;
}

int usage()
{
    fprintf(stderr, "           USAGE:\n \
            ./funcTrace -p clibFile noRetFile parsedDirectory flatTrace\n \
            ./funcTrace -c clibFile noRetFile parsedDirectory flatTrace1 flatTrace2\n \
            ./funcTrace -t clibFile noRetFile parsedDirectory flatTrace\n \
            ./funcTrace -f inputFile\n"); 
    return EXIT_SUCCESS;      
}

int main(int argc, char **argv)
{

    Args args;
    int k = args.processArgs(argc, argv);
    if(k == -1) 
    {
        fprintf(stderr, "processing args failed\n");
        usage();
        return EXIT_FAILURE;
    }
    
    if(args.mode == Mode::READFILE)
    {
        std::cout << "need to read the file first\n";
        args.readFile();
    }

    std::cout << "below is what you are trying to do\n" << args;

    if(args.mode == Mode::HELP)
    {
        return usage();
    }
    else if(args.mode == Mode::COMPARE)
    {
        return pathCompare(args);
    }
    else if(args.mode == Mode::FINDPATH)
    {
        return findAPath(args);
    }
    else if(args.mode == Mode::READFILE)
    {
        fprintf(stderr, "ERROR::Mode is not supposed to be READFILE after the file is read\n");
        usage();
        return EXIT_FAILURE;
    } 
    else if(args.mode == Mode::PRINTTRACE)
    {
        return printHierarchicalPath(args);
    }
    else if(args.mode == Mode::ERR)
    {
        fprintf(stderr, "ERROR::Mode is ERR\n");
        usage();
        return EXIT_FAILURE;
    } 
    else
    {
        fprintf(stderr, "ERROR::Mode is not right at all\n");
        usage();
        return EXIT_FAILURE; 
    }
}
