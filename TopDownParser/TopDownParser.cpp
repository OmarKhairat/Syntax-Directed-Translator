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
        input.push(analyzer.getNextToken().first);
    }
    TopDownParser::input = input;
}

bool TopDownParser::isTerminal(string token)
{
    // return token[0] == '\'' && token[token.length() - 1] == '\'' && token.length() > 2;
    for (int i = 0; i < nonTerminals.size(); i++)
    {
        if (nonTerminals[i].compare(token) == 0)
            return false;
    }
    return true;
}

string TopDownParser::removeQuotes(string token)
{
    // return token.substr(1, token.length() - 2);
    return token;
}

string TopDownParser::addStackToLevel(stack<string> tokenStack, string levelStr)
{
    stack<string> tempStack = tokenStack;
    while (!tempStack.empty())
    {
        levelStr += tempStack.top();
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

    // int size = stk.size();

    // queue<string> q;

    // cout << "STACK" << endl;
    // for (int i = 0; i < size; i++)
    // {
    //     string element = stk.top();
    //     q.push(element);
    //     stk.pop();
    //     cout << endl
    //          << element;
    // }

    // for (int i = 0; i < size; i++)
    // {
    //     string element = q.front();
    //     stk.push(element);
    //     q.pop();
    // }

    // int inputSize = input.size();

    // stack<string> x;

    // cout << endl << "INPUT" << endl;

    // for (int i = 0; i < inputSize; i++)
    // {
    //     string element = input.front();
    //     x.push(element);
    //     input.pop();
    //     cout << endl << element;
    // }

    // for (int i = 0; i < inputSize; i++)
    // {
    //     string element = x.top();
    //     input.push(element);
    //     x.pop();
    // }

    cout << "Is stack 0: " << stk.empty() << endl;
    int iii = 0;
    while (!stk.empty() && !input.empty())
    {
        // cout << "Is stack: "<< stk.empty() << endl;
        string curTreeLevel = matchedTokens.back();
        topToken = stk.top();
        inputToken = input.front();
        cout << iii << " Stack top: " << stk.top() << " Input front: " << input.front() << endl;
        iii++;
        if (isTerminal(topToken))
        {
            cout << "TERMINAL" << endl;
            if (inputToken.compare(removeQuotes(topToken)) == 0)
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
            cout << "NONTERMINAL" << endl;
            unordered_map<string, vector<vector<string>>> topTokenRow = table[topToken];

            bool found = false;

            for (vector<string> vec : topTokenRow[inputToken])
            {
                auto it = find(vec.begin(), vec.end(), inputToken);

                if (it != vec.end())
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                cout << "ROW FOUND IN TABLE" << endl;
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
    if (!stk.empty() || !input.empty())
    {
        cout << "There is something wrong stack:" << stk.empty() << " input:" << input.empty() << endl;
    }

    treeLevels.erase(treeLevels.begin());
    return treeLevels;
}