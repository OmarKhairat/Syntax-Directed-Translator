#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <set>
#include <string>
#include "..\LexicalAnalyzer\LexicalAnalyzer.h"

#ifndef SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
#define SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H

using namespace std;

class TopDownParser
{
public:
    explicit TopDownParser(unordered_map<string, unordered_map<string, vector<vector<string>>>> table, vector<string> nonTerminals, LexicalAnalyzer analyzer);

    vector<string> parse();

    bool isTerminal(string token);

    string addStackToLevel(stack<string> tokenStack, string levelStr);

    stack<string> addProductionRuleToStack(stack<string> tokenStack, vector<string> productionRule);

private:
    unordered_map<string, unordered_map<string, vector<vector<string>>>> table;

    stack<string> stk;

    queue<string> input;

    vector<string> nonTerminals;

};

#endif // SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
