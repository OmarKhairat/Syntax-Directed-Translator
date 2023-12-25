#include "TopDownParser.h"
#include <string>

using namespace std;

TopDownParser::TopDownParser(unordered_map<string, unordered_map<string, vector<string>>> table, vector<string> nonTerminals, LexicalAnalyzer analyzer)
{
    TopDownParser::analyzer = analyzer;
    TopDownParser::table = table;
    TopDownParser::stk.push("\'$\'");
    TopDownParser::stk.push(nonTerminals[0]);
    queue<string> input;
    while(analyzer.hasNextToken())
    {
        input.push(analyzer.getNextToken().second);
    }
    input.push("$");
    TopDownParser::input = input;
}

bool TopDownParser::isTerminal(string token)
{
    return token[0] == '\'' && token[token.length() - 1] == '\'' && token.length() > 2;
}

string TopDownParser::removeQuotes(string token)
{
    return token.substr(1, token.length() - 2);
}

string TopDownParser::addStackToLevel(stack<string> tokenStack, string levelStr)
{
    stack<string> tempStack = tokenStack;
    while (!tempStack.empty()) {
        levelStr += tempStack.top();
        tempStack.pop();
    }
    return levelStr;
}

stack<string> TopDownParser::addProductionRuleToStack(stack<string> tokenStack, vector<string> productionRule)
{
    for(int i =  productionRule.size() - 1; i >= 0; i--)
    {
        tokenStack.push(productionRule[i]);
    }
    return tokenStack;
}

vector<string> TopDownParser::parse(stack<string> stk, queue<string> input)
{
    string topToken, inputToken;
    vector<string> treeLevels;
    vector<string> matchedTokens;
    matchedTokens.push_back("");
    // Iterate until the stack and input converge.
    treeLevels.push_back(stk.top());
    cout << treeLevels.back() << endl;
    while (!stk.empty() && !input.empty())
    {
        string curTreeLevel = matchedTokens.back();
        topToken = stk.top();
        inputToken = input.front();

        if(isTerminal(topToken))
        {
            if(inputToken.compare(removeQuotes(topToken)) == 0)
            {
                curTreeLevel += inputToken;
                matchedTokens.push_back(matchedTokens.back() + inputToken);
                stk.pop();
                input.pop();
                // curTreeLevel = addStackToLevel(stk, curTreeLevel);
                // cout << curTreeLevel << endl;
                // treeLevels.push_back(curTreeLevel);
                // continue;
            }
            else
            {
                string errorMsg = "Error Missing " + topToken + " inserted";
                cout << errorMsg << endl;
                treeLevels.push_back(errorMsg);
                curTreeLevel += removeQuotes(topToken);
                matchedTokens.push_back(matchedTokens.back() + removeQuotes(topToken));
                stk.pop();
            }
            curTreeLevel = addStackToLevel(stk, curTreeLevel);
            cout << curTreeLevel << endl;
            treeLevels.push_back(curTreeLevel);
        }
        else
        {
            unordered_map<string, vector<string> > topTokenRow = table[topToken];
            auto it = topTokenRow.find(inputToken);
            if(it != topTokenRow.end())
            {
                vector<string> productionRule = topTokenRow[inputToken];
                string curTreeLevel = matchedTokens.back();
                if(productionRule[0].compare("\\SYNC\\" ) == 0 || productionRule[0].compare("\\L" ) == 0)
                {
                    if(productionRule[0].compare("\\SYNC\\" ) == 0)
                    {
                        string errorMsg = "Error: Unmatched Terminal Symbol " + topToken;
                        cout << errorMsg << endl;
                        treeLevels.push_back(errorMsg);
                    }
                    else
                    {
                        input.pop();
                    }
                    stk.pop();      
                }
                else
                {
                    stk.pop();
                    stk = addProductionRuleToStack(stk, productionRule);
                }
                curTreeLevel = addStackToLevel(stk, curTreeLevel);
                cout << curTreeLevel << endl;
                treeLevels.push_back(curTreeLevel);
            }
            else
            {
                // Error:(illegal A) – discard e
                string errorMsg = "Error:(illegal " + topToken + ") - discard " + inputToken;
                cout << errorMsg << endl;
                input.pop();
                treeLevels.push_back(errorMsg);
            }
        }
    }
    if(!stk.empty() || !input.empty())
    {
        cout << "There is something wrong stack:" << stk.empty() " input: " << input.empty() << endl;
    }


    treeLevels.erase(treeLevels.begin());
    return treeLevels;
}