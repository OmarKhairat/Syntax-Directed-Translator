#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>

#ifndef SYNTAX_DIRECTED_TRANSLATOR_PARSINGTABLE_H
#define SYNTAX_DIRECTED_TRANSLATOR_PARSINGTABLE_H

using namespace std;

class ParsingTable
{
public:
    explicit ParsingTable(unordered_map<string, vector<string>> grammar,
                          unordered_map<string, vector<string>> firsts,
                          unordered_map<string, vector<string>> follows);

    unordered_map<string, unordered_map<string, vector<string>>> getTable();

private:
    unordered_map<string, vector<string>> grammar;

    unordered_map<string, vector<string>> firstSets;

    unordered_map<string, vector<string>> followSets;

    unordered_map<string, unordered_map<string, vector<string>>> table;

    void constructTable();
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_PARSINGTABLE_H
