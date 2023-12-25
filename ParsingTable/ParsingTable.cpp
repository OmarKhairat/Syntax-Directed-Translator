#include "ParsingTable.h"

ParsingTable::ParsingTable(vector<pair<string, set<vector<string>>>> grammar,
                           unordered_map<string, vector<string>> firstSets,
                           unordered_map<string, vector<string>> followSets,
                           vector<string> nonTerminals)
{
    // Initialize the table, a temporary token, and a vector for productions.
    unordered_map<string, unordered_map<string, vector<vector<string>>>> table;
    string nonTerminal;
    vector<string> firstSet;
    vector<string> followSet;
    set<vector<string>> productions;

    cout << "GENERATING THE PARSING TABLE FIRST SET ENTRIES" << endl;

    // Loop over the elements in the first sets.
    for (auto mapping = firstSets.end(); mapping != firstSets.begin(); --mapping)
    {
        nonTerminal = mapping->first;

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
                        firstSet = mapping->second;

                        // Check if the symbol is a terminal and is in the first set.
                        auto setIt = find(firstSet.begin(), firstSet.end(), symbol);

                        // Check if the symbol is a non-terminal.
                        auto ntIt = find(nonTerminals.begin(), nonTerminals.end(), symbol);

                        if (setIt != firstSet.end())
                        {
                            // If found in the first set (terminal), add it to the parsing table.
                            table[nonTerminal][symbol].emplace_back(production);
                        }
                        else if (ntIt != nonTerminals.end())
                        {
                            // If the first symbol in the production is a non-terminal,
                            // assign the columns of that non-terminal to the current token in the table.
                            for (pair<string, vector<vector<string>>> column : table[symbol])
                            {
                                table[nonTerminal].insert(column);
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
            cerr << "Nonterminal not found." << endl;
        }
    }

    cout << "FIRST SET ENTRIES COMPLETE" << endl;

    cout << "GENERATING THE PARSING TABLE FOLLOW SET ENTRIES" << endl;

    // Loop over the elements in the follow sets.
    for (auto mapping = firstSets.end(); mapping != firstSets.begin(); --mapping)
    {
        nonTerminal = mapping->first;

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
                string epsilon = R"(\L)";

                followSet = mapping->second;

                auto it = find(production.begin(), production.end(), epsilon);

                if (it != production.end())
                {
                    for (string s : followSet)
                    {
                        table[nonTerminal][s].emplace_back(production);
                    }
                }
                else
                {
                    vector<string> sync;
                    sync.emplace_back(R"(\SYNC\)");

                    for (string s : followSet)
                    {
                        table[nonTerminal][s].emplace_back(sync);
                    }
                }
                cout << "| "; // Separator between productions
            }
            cout << endl;
        }
        else
        {
            cerr << "Nonterminal not found." << endl;
        }
    }

    cout << "FOLLOW SET ENTRIES COMPLETE" << endl;

    ParsingTable::table = table;
}

unordered_map<string, unordered_map<string, vector<vector<string>>>> ParsingTable::getTable()
{
    return table;
}