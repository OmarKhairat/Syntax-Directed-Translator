#include "TopDownParser.h"

TopDownParser::TopDownParser(unordered_map<string, unordered_map<string, vector<string>>> table, vector<string> nonTerminals, LexicalAnalyzer analyzer)
{
    TopDownParser::analyzer = analyzer;
    TopDownParser::table = table;
    TopDownParser::stk.push("$");
    TopDownParser::stk.push(nonTerminals[0]);
}

vector<string> parse(stack<string> stk, queue<string> input)
{
    string stackToken, inputToken;

    // Iterate until the stack and input converge.
    while (!stk.empty() && !input.empty())
    {
        stackToken = stk.top();
        stk.pop();

        inputToken = input.front();
        input.pop();
    }
}