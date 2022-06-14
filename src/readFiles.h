// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#ifndef READ_FILES_H
#define READ_FILES_H

//#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "tools.h"
#include "cfg.h"

std::map<std::string, int> file2Dict(std::string file);

std::vector<std::string> makeFlatTrace(std::string traceFile);

std::map<std::string, cfg_t *> readCFGs(char *dirName, ErrorCode& ec);
#endif /* READ_FILES_H */
