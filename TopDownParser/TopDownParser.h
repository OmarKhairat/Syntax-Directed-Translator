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
#include "LexicalAnalyzer.h"

#ifndef SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
#define SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H

using namespace std;

class TopDownParser
{
public:
    explicit TopDownParser(unordered_map<string, unordered_map<string, vector<string>>> table, vector<string> nonTerminals, LexicalAnalyzer analyzer);

    vector<string> parse(stack<string> stk, queue<string> input);
    bool isTerminal(string token);
    string removeQuotes(string token);
    string addStackToLevel(stack<string> tokenStack, string levelStr);
    stack<string> addProductionRuleToStack(stack<string> tokenStack, vector<string> productionRule);

private:
    LexicalAnalyzer analyzer{};

    unordered_map<string, unordered_map<string, vector<string>>> table;

    stack<string> stk;

    queue<string> input;

    vector<string> nonTerminals;

    // vector<string> output;
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
