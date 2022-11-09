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

using namespace std;

// I want to write this function in some namespace or something later
funcTrace *initMainFT(void)
{
    funcTrace *ft = new funcTrace("main");
    return ft;
}

/*
 * this function is called when you need to compare the two paths
 * the path tracing is now done lazily... so that you wouldn't have to deal with large paths and get stuck in the exponential algorithms, the less amount of work, the better
 */
int pathCompare(Args& args)
{
    ErrorCode ec = ErrorCode::SUCCESS;
    cout << "reading cfgs\n";
    char *tmpStr = (char *)malloc(strlen(args.parsedDirectory.c_str()) + 1);
    strcpy(tmpStr, args.parsedDirectory.c_str());

    /*
     * this part is reading cfgs that are created with llvmanalysis repo
     */
    map<std::string, cfg_t *>cfgs = readCFGs(tmpStr, ec); 
    free(tmpStr);
    //Tools::print_map(cfgs);

    /*
     * this part is reading the clibFile which states which functions are relevant to system calls
     */
    std::cout << "making dict\n";
    std::map<std::string, int> clibDict = file2Dict(args.clibFile);    

    /*
     * this part is creating the flat trace without the consideration of function hierarchy from the trace that's captured preliminarily.
     */
    //Tools::print_map(clibDict);
    cout << "making flatTrace\n";
    vector<string> flatTrace1 = makeFlatTrace(args.flatTrace1); 
    //print_vector(flatTrace);
    vector<string> flatTrace2 = makeFlatTrace(args.flatTrace2); 

    /*
     * this part is reading the functions that did not have return statement, which creates more hassle to create function level trace from the flat trace we just read
     */
    map<string, int> noRetFuncs = file2Dict(args.noRetFile);

    /*
     * the below is where function trace is being made hierarchical instead of flat
     */
    auto start1 = chrono::steady_clock::now();
    funcTrace *ft1 = new funcTrace();
    ft1->makeFuncTrace(flatTrace1, clibDict, noRetFuncs, cfgs);
    ft1->updateHash();
    auto end1 = chrono::steady_clock::now();

    auto start2 = chrono::steady_clock::now();
    funcTrace *ft2 = new funcTrace();
    ft2->makeFuncTrace(flatTrace2, clibDict, noRetFuncs, cfgs);
    ft2->updateHash();
    auto end2 = chrono::steady_clock::now();

    cout << "flatTrace1 functionTrace make time: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count()
        << " microseconds\n";

    cout << "flatTrace2 functionTrace make time: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count()
        << " microseconds\n";

    /*
     * below is where hierarchical function trace is being compared with the other hierarchical function trace
     */
    cout << "comparison has started" << std::endl;
    long time = 0;
    int diff = 0;
    auto tik = chrono::steady_clock::now();
    ft1->ftcmp(ft2, cfgs, time, diff);
    auto tok = chrono::steady_clock::now();
    cout << "comparison time: "
        << chrono::duration_cast<std::chrono::microseconds>(tok - tik).count()
        << " microseconds\n";
    cout << "path tracing time: "
        << time << " microseconds\n";
    
    /*
     * reporting the number of divergence in the trace
     */
    cout << diff << " divergence were there\n";

    delete ft1; 
    delete ft2;
    map<string, cfg_t *>::iterator it;
    for(it = cfgs.begin(); it != cfgs.end(); it++)
    {
        delete it->second;
    }
    
    return EXIT_SUCCESS;
}

/*
 * this function rarely works due to the path explosion problem.
 * to iterate through all the possible paths, it would take a exponential time and to do that, even with hierarchical function trace, it takes significant amount of time.
 */
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

/*
 * This function finds a path from the beggining of the trace to the end.
 * Since this method does not allow the string level comparison of 2 traces, which enables lazy path tracing, it would take more time to trace the paths, but it usually works despite the fact in the worst case, it could face the path explosion proble.
 */
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

/*
 * This part is printing the hierarchical function trace using indent to show the hierarchy
 */
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

/*
 * this function is (obviously) showing the usage of the program
 * below in the fprintf functions are the usage of the program
 */
int usage()
{
    fprintf(stderr, "           USAGE:\n \
            ./funcTrace -p clibFile noRetFile parsedDirectory flatTrace\n \
            ./funcTrace -c clibFile noRetFile parsedDirectory flatTrace1 flatTrace2\n \
            ./funcTrace -t clibFile noRetFile parsedDirectory flatTrace\n \
            ./funcTrace -f inputFile\n"); 
    return EXIT_SUCCESS;      
}

/*
 * this is main function, all it does is pass the arguments to each options and according to the options given, does path-finding, comparison, printing a hierarchical path, printing a usage of the program
 */
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
