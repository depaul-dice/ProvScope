// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#include "tools.h"

namespace Tools
{

    char *extractSubstring(char *filename)
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

}
