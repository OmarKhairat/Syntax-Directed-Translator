#include "ParsingTable.h"

ParsingTable::ParsingTable(vector<pair<string, set<vector<string>>>> grammar,
                           unordered_map<string, vector<string>> firstSets,
                           unordered_map<string, vector<string>> followSets,
                           set<string> nonTerminals)
{
    // Initialize the table, a temporary token, and a vector for productions.
    unordered_map<string, unordered_map<string, vector<string>>> table;
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
                              return element.first == nonTerminal;
                          });

        if (it != grammar.end())
        {
            productions = it->second;

            // Printing the token and its productions (if found).
            cout << nonTerminal << "-->";
            for (string p : productions)
            {
                cout << p;

                // Check the occurence of the first element in the first set
                // in the current production while you're at it.
                istringstream iss(p);

                string s;

                iss >> s;

                firstSet = mapping->second;

                auto setIt = find(firstSet.begin(), firstSet.end(), s);
                auto ntIt = find(nonTerminals.begin(), nonTerminals.end(), s);

                if (setIt != firstSet.end())
                {
                    // If found, add it to the parsing table.
                    table[nonTerminal][s].emplace_back(p);
                }
                else if (ntIt != nonTerminals.end())
                {
                    // If the first character in the production is a non-terminal,
                    // assign the columns of that non-terminal to the current token in the table.
                    for (pair<string, vector<string>> column : table[s])
                    {
                        table[nonTerminal].insert(column);
                    }
                }
            }
            cout << endl;
        }

        else
        {
            cerr << "No productions found for such token" << endl;
        }
    }

    cout << "FIRST SET ENTRIES COMPLETE" << endl;

    cout << "GENERATING THE PARSING TABLE FOLLOW SET ENTRIES" << endl;

    // Loop over the elements in the follow sets.
    for (pair<string, vector<string>> mapping : followSets)
    {
        nonTerminal = mapping.first;

        // Look for the productions of the current token.
        auto it = find_if(grammar.begin(), grammar.end(),
                          [nonTerminal](const auto &element)
                          {
                              return element.first == nonTerminal;
                          });

        if (it != grammar.end())
        {
            productions = it->second;

            // Printing the token and its productions (if found).
            cout << nonTerminal << "-->";

            // TODO: Handle this.
            for (string p : productions)
            {
                cout << p;

                // In this step we only care about the epsilon production.
                // If the current token has an epsilon production, add it
                // to the table entry of which the column index is an ele-
                // ment in the follow set of the current non-terminal.

                if (p == R"(\L)")
                {
                    followSet = mapping.second;

                    for (string s : followSet)
                    {
                        table[nonTerminal][s].emplace_back(p);
                    }
                }
            }
            cout << endl;
        }

        else
        {
            cerr << "No productions found for such token" << endl;
        }
    }

    cout << "FOLLOW SET ENTRIES COMPLETE" << endl;

    ParsingTable::table = table;
}

unordered_map<string, unordered_map<string, vector<string>>> ParsingTable::getTable()
{
    return table;
}