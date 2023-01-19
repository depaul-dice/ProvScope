// Copyright (c) 2021 Yuta Nakamura. All rights reserved

#include "Args.h"

using namespace std;

Mode Args::parseMode(char *arg, int argc)
{
    while(*arg == ' ') arg++;

    if(strcmp(arg, "-c") == 0 && argc == 7)
        return Mode::COMPARE;
    else if(strcmp(arg, "-p") == 0 && argc == 6)
        return Mode::FINDPATH;
    else if(strcmp(arg, "-f") == 0 && argc == 3)
        return Mode::READFILE;
    else if(strcmp(arg, "-t") == 0 && argc == 6)
        return Mode::PRINTTRACE;
    else if(strcmp(arg, "-e") == 0 && argc == 7)
        return Mode::EXPERIMENT;
    else if(strcmp(arg, "-s") == 0 && argc == 4)
        return Mode::SPEC;
    else if(strcmp(arg, "-h") == 0 && argc == 2)
        return Mode::HELP;
    else {
        fprintf(stderr, "could not process mode correctly\narg: %s, argc: %d\n", arg, argc);
        return Mode::ERR;
    }
}

// returns 0 with success
int Args::processArgs(int argc, char **argv)
{
    for(int i = 0; i < argc; i++) {
        switch(i) {
            case 0:
                break;
            case 1:
                mode = parseMode(argv[i], argc);
                if(mode == Mode::ERR)
                    return -1;
                break;
            case 2:
                if(mode == Mode::READFILE) filename = argv[i];
                else if(mode == Mode::SPEC) parsedDirectory = argv[i];
                else clibFile = argv[i];
                break;
            case 3:
                if(mode == Mode::SPEC) outfile = argv[i];
                else noRetFile = argv[i];        
                break;
            case 4:
                parsedDirectory = argv[i];
                break;
            case 5:
                flatTrace1 = argv[i];
                break;
            case 6:
                flatTrace2 = argv[i];
                break;
            default:
                mode = Mode::ERR;
                return -1;
        }
    }

    if(mode == Mode::HELP && argc != 2) {
        fprintf(stderr, "help format not correct\n");
        return -1;
    }
    if(mode == Mode::READFILE && argc != 3) {
        fprintf(stderr, "readfile format not correct\n");
        return -1;
    } else if(mode == Mode::FINDPATH && argc != 6) {
        fprintf(stderr, "findpath format not correct\n");
        return -1;
    } else if((mode == Mode::COMPARE || mode == Mode::EXPERIMENT) && argc != 7) {
        fprintf(stderr, "comparison format not correct\n");
        return -1;
    } else if(mode == Mode::PRINTTRACE && argc != 6) {
        fprintf(stderr, "print trace format not correct\n");
        return -1;
    }

    return 0; 
}

// the function below is from https://stackoverflow.com/questions/9358718/similar-function-in-c-to-pythons-strip
// this is basically python strip function
void Args::strip(std::string &str)
{
    if(str.length() == 0)
        return;
    auto w = std::string(" ");
    auto n = std::string("\n");
    auto r = std::string("\t");
    auto t = std::string("\r");
    auto v = std::string(1, str.front());
    while(v == w || v == t || v == r || v == n)
    {
        str.erase(str.begin());
        v = std::string(1, str.front());
    }

    v = std::string(1, str.back());
    while(v == w || v == t || v == r || v == n)
    {
        str.erase(str.end() - 1);
        v = std::string(1, str.back());
    }
}

// helper function to parse the input 
std::vector<std::string> Args::getInput(std::string inputFile)
{
    std::vector<std::string> arguments;

    std::ifstream inFile;
    std::string tmp;

    inFile.open(inputFile);
    do
    {
        getline(inFile, tmp, ' ');
        if(tmp.length() > 0)
        {
            arguments.push_back(tmp);
        }
    }
    while(inFile.eof() == false);
    inFile.close();    

    for(unsigned i = 0; i < arguments.size(); i++)
    {
        strip(arguments[i]);
    }

    return arguments;
}

int Args::readFile(void)
{
    char *tmpStr = (char *)malloc(1024);
    std::vector<std::string> arguments = getInput(this->filename);

    for(unsigned i = 0; i < arguments.size(); i++)
    {
        switch(i) 
        {
            case 0:
                break;
            case 1:
                strcpy(tmpStr, arguments[i].c_str());

                mode = parseMode(tmpStr, (int)arguments.size());
                if(mode == Mode::ERR) return -1;
                break;
            case 2:
                if(mode == Mode::READFILE) filename = arguments[i];
                else clibFile = arguments[i];
                break;
            case 3:
                noRetFile = arguments[i];        
                break;
            case 4:
                parsedDirectory = arguments[i];
                break;
            case 5:
                flatTrace1 = arguments[i];
                break;
            case 6:
                flatTrace2 = arguments[i];
                break;
            default:
                mode = Mode::ERR;
                return -1;
        }
    }
    free(tmpStr);

    return EXIT_SUCCESS;
}


ostream& operator << (ostream& os, Args const& args)
{
    if(args.mode == Mode::COMPARE || args.mode == Mode::EXPERIMENT) {
        os << "mode: COMPARE" 
            << "\nclibfile: " << args.clibFile
            << "\nnoRetFile: " << args.noRetFile
            << "\nparsedDirectory: " << args.parsedDirectory
            << "\nflatTrace1: " << args.flatTrace1 
            << "\nflatTrace2: " << args.flatTrace2 << '\n';
    } else if(args.mode == Mode::FINDPATH) {
        os << "mode: FINDPATH" 
            << "\nclibfile: " << args.clibFile
            << "\nnoRetFile: " << args.noRetFile
            << "\nparsedDirectory: " << args.parsedDirectory
            << "\nflatTrace: " << args.flatTrace1 << '\n';
            
    } else if(args.mode == Mode::READFILE) {
        // I should add a way to read the file later
        os << "mode: READFILE" 
            << "filename: " << args.filename << '\n';
    } else if(args.mode == Mode::PRINTTRACE) {
        os << "mode: PRINTTRACE"
            << "\nclibfile: " << args.clibFile
            << "\nnoRetFile: " << args.noRetFile
            << "\nparsedDirectory: " << args.parsedDirectory
            << "\nflatTrace: " << args.flatTrace1 << '\n';
    } else {
        os << "mode: ERR\n";
    }
    return os;
    
}
