
#ifndef COMPARISON_H
#define COMPARISON_H

#include "Matrix.h"
#include "cfg.h"
#include "ec.h"
#include <cassert>
#include <cstring>
#include <vector>
#include <map>
#include <tuple>

// todo figure out how to add alignedPairs and diff 
class Comparison
{
public:
    std::vector<node *>& hpath;
    std::vector<node *>& vpath;

    Comparison() = delete;
    Comparison(const Comparison&) = delete;
    Comparison& operator = (const Comparison&) = delete;
    ~Comparison() = default;

    //Comparison(std::vector<node *>& path1, std::vector<node *>& path2);
    Comparison(std::vector<node *>& path1, std::vector<node *>& path2, std::vector<std::tuple<node *, unsigned>>& loopPath1, std::vector<std::tuple<node *, unsigned>>& loopPath2);
    
    void printMatrix(std::ostream&);

    ErrorCode editDistance(std::vector<std::tuple<unsigned, unsigned>>& alignedPairs);    
    void greedyApproach(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff);

    ErrorCode loopGreedyApproach(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff);

private:
    const float insertCost = 2;
    const float alternateCost = 2;
    std::vector<std::tuple<node *, unsigned>>& loopPath1;
    std::vector<std::tuple<node *, unsigned>>& loopPath2;

    Matrix matrix;

    void editDistanceInitSetUp(void);
    float smallest(float, float, float);
    ErrorCode returnEC(unsigned , unsigned);

    ErrorCode __loopGreedyContents(std::vector<std::tuple<node *, unsigned>>& content1, std::vector<std::tuple<node *, unsigned>>& content2, std::vector<std::tuple<unsigned, unsigned>> &alignedPairs, int &diff);
    ErrorCode findMinCostTrace(std::vector<std::tuple<unsigned, unsigned>> &alignedPairs);
    ErrorCode fillMatrix(void);
    bool nodeMatch(node *a, node *b);
   
};

#endif /* COMPARE_H */
