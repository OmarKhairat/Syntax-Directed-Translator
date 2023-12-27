#include "ParsingTable.h"

using namespace std;

ParsingTable::ParsingTable(vector<pair<string, set<vector<string>>>> grammar,
                           vector<pair<string, vector<string>>> firstSets,
                           vector<pair<string, vector<string>>> followSets,
                           vector<string> nonTerminals)
{
    // Initialize the table, a temporary token, and a vector for productions.
    unordered_map<string, unordered_map<string, vector<vector<string>>>> constructedTable;
    string nonTerminal;
    vector<string> firstSet;
    vector<string> followSet;
    set<vector<string>> productions;

    cout << "GENERATING THE PARSING TABLE FIRST SET ENTRIES" << endl;

    // Loop over the elements in the first sets.
    for (int i = 0; i < firstSets.size(); ++i)
    {
        pair<string, vector<string>> mapping = firstSets[i];
        nonTerminal = mapping.first;
        firstSet = mapping.second;

        // Look for the productions of the current token.
        auto it = find_if(grammar.begin(), grammar.end(),
                          [nonTerminal](const auto &element)
                          {
                              return element.first.compare(nonTerminal) == 0;
                          });

        // Check if the pair.first was found
        if (it != grammar.end())
        {
            // Access the set<vector<string>> associated with the found pair
            const auto &productions = it->second;

            // Print the results
            cout << it->first << " --> ";
            for (const auto &production : productions)
            {
                for (auto pIt = production.begin(); pIt < production.end(); pIt++)
                {
                    string symbol = *pIt;

                    cout << symbol << " ";

                    if (pIt == production.begin())
                    {
                        // Check the occurence of the first element in the first set
                        // in the current production while you're at it.

                        // Check if the symbol is a terminal and is in the first set.
                        auto setIt = find(firstSet.begin(), firstSet.end(), symbol);

                        if (setIt != firstSet.end() && symbol.compare(R"(\L)") != 0)
                        {
                            // If found in the first set (terminal), add its corresponding production to the parsing table.
                            constructedTable[nonTerminal][symbol].emplace_back(production);
                        }
                        else
                        {
                            // The symbol is non-terminal. Get its first set and add its corresponding production to the first set.
                            auto ntSetIt = find_if(firstSets.begin(), firstSets.end(),
                                                   [symbol](const auto &element)
                                                   {
                                                       return element.first.compare(symbol) == 0;
                                                   });

                            if (ntSetIt != firstSets.end())
                            {
                                // If found the first set of the non-terminal,
                                // add the current production to each entry in that set.
                                for (string s : ntSetIt->second)
                                {
                                    auto findCurrentToken = find(firstSet.begin(), firstSet.end(), s);

                                    // Add the production only to the intersection between the two firstSets.
                                    if (findCurrentToken != firstSet.end())
                                    {
                                        constructedTable[nonTerminal][s].emplace_back(production);
                                    }
                                }
                            }
                        }
                    }
                }
                cout << "| "; // Separator between productions
            }
            cout << endl;
        }
        else
        {
            cout << "Nonterminal not found." << endl;
        }
    }

    cout << "FIRST SET ENTRIES COMPLETE" << endl;

    cout << "GENERATING THE PARSING TABLE FOLLOW SET ENTRIES" << endl;

    ofstream op("op.txt");

    // Loop over the elements in the follow sets.
    for (int i = followSets.size() - 1; i > -1; --i)
    {
        pair<string, vector<string>> mapping = followSets[i];

        nonTerminal = mapping.first;

        // Look for the productions of the current token.
        auto it = find_if(grammar.begin(), grammar.end(),
                          [nonTerminal](const auto &element)
                          {
                              return element.first.compare(nonTerminal) == 0;
                          });

        // Check if the pair.first was found
        if (it != grammar.end())
        {
            // Access the set<vector<string>> associated with the found pair
            const auto &productions = it->second;

            // Print the results
            cout << it->first << " --> ";

            for (const auto &production : productions)
            {
                for (const auto &symbol : production)
                {
                    cout << symbol << " ";
                }

                followSet = mapping.second;

                string epsilon = R"(\L)";

                // Check if the first symbol in the production is a non-terminal.
                string firstToken = production[0];

                auto ntIt = find(nonTerminals.begin(), nonTerminals.end(), firstToken);

                // Check if the production is an epsilon.
                auto epsIt = find(production.begin(), production.end(), epsilon);

                if (epsIt != production.end())
                {
                    // If the production is an epsilon.
                    for (string s : followSet)
                    {
                        if (s.compare("") != 0)
                        {
                            constructedTable[nonTerminal][s].emplace_back(production);
                        }
                    }
                }
                else if (ntIt == production.end())
                {
                    // If the production is not an epsilon.
                    vector<string> sync;
                    sync.emplace_back(R"(\SYNC\)");

                    for (string s : followSet)
                    {
                        constructedTable[nonTerminal][s].emplace_back(sync);
                    }
                }
                cout << "| "; // Separator between productions
            }
            cout << endl;
        }
        else
        {
            cout << "Nonterminal not found." << endl;
        }
    }

    cout << "FOLLOW SET ENTRIES COMPLETE" << endl;

    ParsingTable::table = constructedTable;
}

unordered_map<string, unordered_map<string, vector<vector<string>>>> ParsingTable::getTable()
{
    return table;
}