#ifndef FSA_H
#define FSA_H

#include <bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define sz(a) int((a).size())
#define pb push_back
#define all(c) (c).begin(),(c).end()
#define tr(c,i) for(typeof((c).begin()) i = (c).begin(); i != (c).end(); i++)
#define present(c,x) ((c).find(x) != (c).end())
#define cpresent(c,x) (find(all(c),x) != (c).end())

void insertAlpha(std::string str);

void construct(std::vector<std::vector<std::string>>& input,int tail,std::vector<std::unordered_map<int, std::unordered_set<int>>>& transition_table);

void printAlpha();

#endif /* FSA_H */
