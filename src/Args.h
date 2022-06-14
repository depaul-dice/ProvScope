// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#ifndef ARGS_H
#define ARGS_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

enum class Mode
{
    HELP,
    COMPARE,
    FINDPATH,
    READFILE,
    PRINTTRACE,
    ERR,
};

class Args
{
public:
    // big 4
    Args() = default;
    Args(const Args&) = delete;
    Args& operator = (const Args&) = delete;
    ~Args() = default;

    int processArgs(int argc, char **argv);
    int readFile(void);
    friend std::ostream& operator << (std::ostream& os, Args const& args);

    Mode mode;
    std::string filename; // this is a filename for READFILE mode 
    std::string clibFile;
    std::string noRetFile;
    std::string parsedDirectory;
    std::string flatTrace1;
    std::string flatTrace2;

private:
    Mode parseMode(char *arg, int argc);
    std::vector<std::string> getInput(std::string inputFile);
    void strip(std::string &); // this is basically python strip for strings
};

#endif /* ARGS_H */
