// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#ifndef TOOLS_H
#define TOOLS_H

#include <cstddef>
#include <cstring>

#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <iostream>

//#include "funcTrace.h"

//class funcTrace; // forward declaration to deal with cyclical declaration

namespace Tools
{

    char *extractSubstring(char *filename);

    template <typename E>
    void print_set (std::set<E> const &m)
    {
        for(auto it = m.begin(); it != m.end(); it++)
        {
            std::cout << *it << std::endl;
        }
        
    }

    template <typename E>
    void print_set(std::set<E *> const &m)
    {
        for(auto it = m.begin(); it != m.end(); it++)
        {
            if(*it != nullptr)
                std::cout << **it << std::endl;
            else
                std::cout << "null" << std::endl;
        }
    }

    template <typename E>
    std::ostream& operator << (std::ostream& os, const std::set<E> S)
    {
        os << "{";
        for(auto itr = S.begin(); itr != S.end(); itr++)
        {
            os << *itr << ", ";
        }
        os << "}";
        return os;
    }

    template <typename K, typename V>
    void print_map (std::map<K, V> const &m)
    {
        for (auto const &pair: m) 
        {
            std::cout << "{" << pair.first << ": " << pair.second << "}\n";
        }
    }

    template <typename K, typename V>
    void print_map (std::map<K *, std::set<V *>> const &m)
    {
        for(auto const &pair: m)
        {
            std::cout << *pair.first << "-> \n{";
            print_set(pair.second);
            std::cout << "}" << std::endl;
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

    template <typename K, typename V>
    inline bool isContained(K key, std::map<K, V> table)
    {
        auto it = table.find(key);
        if(it != table.end())
            return true;
        else 
            return false;
    }

    template <typename K, typename V>
    inline bool isContained(K key, std::unordered_map<K, V> table)
    {
        auto it = table.find(key);
        if(it != table.end())
            return true;
        else
            return false;
    }

    template <typename K>
    inline bool isContained(K key, std::set<K> table)
    {
        auto it = table.find(key);
        if(it != table.end())
            return true;
        else
            return false;
    }

    template <typename K>
    inline bool isContained(std::set<K> small, std::set<K> big)
    {
        for(auto it = small.begin(); it != small.end(); it++)
        {
            if(!isContained(*it, big)) return false;
        }
        return true;
    }


    /*
     * implementing the set difference
     */
    template <typename E> 
    std::set<E> diff(std::set<E> first, const std::set<E> &second)
    {
        for(auto it = second.begin(); it != second.end(); it++)
        {
            if(isContained(*it, first))
            {
                first.erase(*it);
            }
        }
        return first;
    }

}

#endif /* TOOLS_H */
