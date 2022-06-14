// Copyright (c) 2021 Yuta Nakamura. All rights reserved.
#include "readFiles.h"

std::map<std::string, int> file2Dict(std::string file)
{
    std::map<std::string, int> dict;

    std::ifstream inFile;
    std::string tmp;
    
    inFile.open(file);

    if(!inFile.is_open()) 
    {
        fprintf(stderr, "opening file failed: %s\n", file.c_str());
        exit(1);
    }

    do 
    {
        getline(inFile, tmp);
        if(tmp.length() > 0) 
        {
            dict[tmp] = 1;
        }

    } 
    while(inFile.eof() == false);
    inFile.close();

    return dict;
}


std::vector<std::string> makeFlatTrace(std::string traceFile)
{
    std::vector<std::string> flatTrace;
    
    std::ifstream inFile;
    std::string tmp;

    inFile.open(traceFile.c_str());
    
    if(!inFile.is_open())
    {
        fprintf(stderr, "opening traceFile failed: %s\n", traceFile.c_str());
        exit(1);
    }

    do
    {
        getline(inFile, tmp);
        if(tmp.length() > 0)
        {
            flatTrace.push_back(tmp);
        }
    }
    while(inFile.eof() == false);
    inFile.close();

    return flatTrace;
}

std::map<std::string, cfg_t *> readCFGs(char *dirName, ErrorCode& ec)
{
    std::map<std::string, cfg_t *> cfgs;
    struct dirent *dp;
    DIR *dfd = opendir(dirName);
    if(!dfd)
    {
        fprintf(stderr, "Can't open %s\n", dirName);
        ec = ErrorCode::DIROPEN;
        return cfgs;
    }
    char filename[PATH_MAX];
    char *name;

    while((dp = readdir(dfd)))
    {
        struct stat stbuf;
        
        sprintf(filename, "%s/%s", dirName, dp->d_name);

        if(stat(filename, &stbuf) == -1)
        {
            fprintf(stderr, "Unable to stat file: %s\n", filename);
            continue;
        }
        if((stbuf.st_mode & S_IFMT) == S_IFDIR) 
        {
            continue;
        }
        else
        {
            cfg_t *cfg = new cfg_t(filename);
            name = Tools::extractSubstring(filename);
            cfgs[(std::string)name] = cfg;
            //cfgs.insert(make_pair((string)name, cfg));
            //map<string, cfg_t *>::iterator it;

            /*
            cout << "created cfg, now printing..." << cfg->name << endl;
            for(it = cfgs.begin(); it != cfgs.end(); it++)
            {
                cout << it->first << endl << *(it->second) << endl;
                it->second->clearTraverse();
            }
            */
        }


    }
    /*
    std::map<std::string, cfg_t *>::iterator it;
    for(it = cfgs.begin(); it != cfgs.end(); it++)
    {
        std::cout << it->first << std::endl << *(it->second) << std::endl;
        it->second->clearTraverse();
    }
    */
    closedir(dfd);

    return cfgs;
}


