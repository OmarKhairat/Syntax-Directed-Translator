#include "TopDownParser.h"

using namespace std;

TopDownParser::TopDownParser(unordered_map<string, unordered_map<string, vector<vector<string>>>> table, vector<string> nonTerminals, LexicalAnalyzer analyzer)
{
    TopDownParser::table = table;
    TopDownParser::stk.push("$");
    TopDownParser::nonTerminals = nonTerminals;
    TopDownParser::stk.push(nonTerminals[0]);
    queue<string> input;
    while (analyzer.hasNextToken())
    {
        string nextToken = analyzer.getNextToken().first;
        if (nextToken.compare("assign") == 0)
        {
            input.push("=");
        }
        else
        {
            input.push(nextToken);
        }
    }
    input.push("$");
    TopDownParser::input = input;
}

bool TopDownParser::isTerminal(string token)
{
    if (token.compare("=") == 0)
        return true;
    for (int i = 0; i < nonTerminals.size(); i++)
    {
        if (nonTerminals[i].compare(token) == 0)
            return false;
    }
    return true;
}

string TopDownParser::addStackToLevel(stack<string> tokenStack, string levelStr)
{
    stack<string> tempStack = tokenStack;
    while (!tempStack.empty())
    {
        levelStr += tempStack.top() + " ";
        tempStack.pop();
    }
    return levelStr;
}

stack<string> TopDownParser::addProductionRuleToStack(stack<string> tokenStack, vector<string> productionRule)
{
    for (int i = productionRule.size() - 1; i >= 0; i--)
    {
        tokenStack.push(productionRule[i]);
    }
    return tokenStack;
}

vector<string> TopDownParser::parse()
{
    cout << "Stack top " << stk.top() << endl;
    cout << "input front " << input.front() << endl;
    cout << "input size " << input.size() << endl;
    string topToken, inputToken;
    vector<string> treeLevels;
    vector<string> matchedTokens;
    matchedTokens.push_back("");

    // Iterate until the stack and input converge.
    treeLevels.push_back(stk.top());
    cout << treeLevels.back() << endl;
    int iii = 0;
    while (!stk.empty() && !input.empty())
    {
        string curTreeLevel = matchedTokens.back();
        topToken = stk.top();
        inputToken = input.front();
        iii++;
        cout << iii << ",  TopToken: " << topToken << ", inputToken: " << inputToken << endl;
        if (isTerminal(topToken))
        {
            if (inputToken.compare(topToken) == 0)
            {
                curTreeLevel += inputToken + " ";
                matchedTokens.push_back(matchedTokens.back() + inputToken + " ");
                stk.pop();
                input.pop();
            }
            else
            {
                string errorMsg = "Error Missing " + topToken + " inserted";
                cout << errorMsg << endl;
                treeLevels.push_back(errorMsg);
                curTreeLevel += topToken + " ";
                matchedTokens.push_back(matchedTokens.back() + topToken + " ");
                stk.pop();
            }
            curTreeLevel = addStackToLevel(stk, curTreeLevel);
            cout << curTreeLevel << endl;
            treeLevels.push_back(curTreeLevel);
        }
        else
        {
            unordered_map<string, vector<vector<string>>> topTokenRow = table[topToken];

            auto it = topTokenRow.find(inputToken);

            bool found = it != topTokenRow.end();

            if (found)
            {
                vector<string> productionRule = topTokenRow[inputToken][0];
                string curTreeLevel = matchedTokens.back();
                if (productionRule[0].compare("\\SYNC\\") == 0 || productionRule[0].compare("\\L") == 0)
                {
                    if (productionRule[0].compare("\\SYNC\\") == 0)
                    {
                        string errorMsg = "Error: Unmatched Terminal Symbol " + topToken;
                        cout << errorMsg << endl;
                        treeLevels.push_back(errorMsg);
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
    if (!stk.empty() || !input.empty())
    {
        cout << "There is something wrong stack:" << stk.empty() << " input:" << input.empty() << endl;
    }

    treeLevels.erase(treeLevels.begin());
    return treeLevels;
}