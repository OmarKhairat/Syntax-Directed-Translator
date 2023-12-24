#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
#include <queue>

#ifndef SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
#define SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H

using namespace std;

class TopDownParser
{
public:
    explicit TopDownParser(unordered_map<string, unordered_map<string, vector<string>>> table);

private:
    unordered_map<string, unordered_map<string, vector<string>>> table;

    stack<string> stk;
    
    queue<string> input;
    
    vector<string> output;
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
