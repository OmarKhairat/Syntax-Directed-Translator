#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>

#ifndef SYNTAX_DIRECTED_TRANSLATOR_PARSINGTABLE_H
#define SYNTAX_DIRECTED_TRANSLATOR_PARSINGTABLE_H

using namespace std;

class ParsingTable
{
public:
    explicit ParsingTable(vector<pair<string, set<vector<string>>>> grammar,
                          unordered_map<string, vector<string>> firsts,
                          unordered_map<string, vector<string>> follows,
                          set<string> nonTerminals);

    unordered_map<string, unordered_map<string, vector<string>>> getTable();

private:
    unordered_map<string, unordered_map<string, vector<string>>> table;
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_PARSINGTABLE_H
