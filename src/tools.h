// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#ifndef TOOLS_H
#define TOOLS_H

#include <cstddef>
#include <cstring>

#include <map>
#include <vector>
#include <iostream>

//#include "funcTrace.h"

//class funcTrace; // forward declaration to deal with cyclical declaration

namespace Tools
{

    char *extractSubstring(char *filename);

    template <typename K, typename V>
    void print_map (std::map<K, V> const &m)
    {
        for (auto const &pair: m) 
        {
            std::cout << "{" << pair.first << ": " << pair.second << "}\n";
        }
    }

    template <typename E>
    void print_vector (std::vector<E> const &m)
    {
        for(unsigned i = 0; i < m.size(); i++)
        {
            std::cout << m[i] << std::endl;
        }
    }


    //void print_vector (std::vector<funcTrace *> const &m);

}

#endif /* TOOLS_H */
