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

    /*
    template <typename K, typename V>
    void print_map (std::map<K, V> const &m)
    {
        for (auto const &pair: m) 
        {
            std::cout << "{" << pair.first << ": " << pair.second << "}\n";
        }
    }
    */

    /*
    template <typename E>
    void print_vector (std::vector<E> const &m)
    {
        for(unsigned i = 0; i < m.size(); i++)
        {
            std::cout << m[i] << std::endl;
        }
    }
    */

    /*
    void print_vector (std::vector<funcTrace *> const &m)
    {
        for(unsigned i = 0; i < m.size(); i++) 
        {
            std::cout << m[i]->getFuncName() << std::endl;
        }
    }
    */

}
