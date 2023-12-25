#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <set>

#ifndef SYNTAX_DIRECTED_TRANSLATOR_GRAMMARSORTER_H
#define SYNTAX_DIRECTED_TRANSLATOR_GRAMMARSORTER_H

using namespace std;

class GrammarSorter
{
public:
    explicit GrammarSorter(vector<pair<string, set<vector<string>>>> grammar, set<string> nonTerminals);

private:
    vector<pair<string, set<vector<string>>>> grammar;
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_GRAMMARSORTER_H
