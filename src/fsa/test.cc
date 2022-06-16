#include "fsa.h"
using namespace std;

int main(int argc, char **argv) {
    
    insertAlpha("10");
    insertAlpha("20");

    vector<unordered_map<int, unordered_set<int>>> transition_table; 
    vector<string> path1{"10", "20"}; 
    vector<string> path2{"10", "10", "20"};
    
    vector<vector<string>> input{path1, path2};

    construct(input, 1, transition_table);
    for(int i = 0; i < transition_table.size(); i++) {
        cout << "at " << i << endl;
        for(auto element: transition_table[i]) {
            cout << element.first << "->";
            for(auto itr = element.second.begin(); itr != element.second.end(); itr++) {
                cout << *itr << ", ";
            }
            cout << endl;
        }
    }
}
