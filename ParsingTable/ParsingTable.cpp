#include "ParsingTable.h"

ParsingTable::ParsingTable(unordered_map<string, vector<string>> grammar,
                           unordered_map<string, vector<string>> firsts,
                           unordered_map<string, vector<string>> follows)
{
    // Assign the values to the class.
    ParsingTable::grammar = grammar;
    ParsingTable::firstSets = firsts;
    ParsingTable::followSets = follows;

    // Construct the table.
    constructTable();
}

void ParsingTable::constructTable()
{
    // Initialize the table, a temporary token, and a vector for productions.
    unordered_map<string, unordered_map<string, vector<string>>> table;
    string nonTerminal;
    vector<string> set;
    vector<string> productions;

    cout << "GENERATING THE PARSING TABLE FIRST SET ENTRIES" << endl;

    // Loop over the elements in the first sets.
    for (pair<string, vector<string>> firstSet : firstSets)
    {
        nonTerminal = firstSet.first;

        // Look for the productions of the current token.
        auto it = grammar.find(nonTerminal);

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
                string s = p.substr(0, 1);

                set = firstSet.second;

                auto setIt = find(set.begin(), set.end(), s);

                if (setIt != set.end())
                {
                    // If found, add it to the parsing table.
                    table[nonTerminal][s].emplace_back(p);
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
    for (pair<string, vector<string>> followSet : followSets)
    {
        nonTerminal = followSet.first;

        // Look for the productions of the current token.
        auto it = grammar.find(nonTerminal);

        if (it != grammar.end())
        {
            productions = it->second;

            // Printing the token and its productions (if found).
            cout << nonTerminal << "-->";
            for (string p : productions)
            {
                cout << p;

                // In this step we only care about the epsilon production.
                // If the current token has an epsilon production, add it
                // to the table entry of which the column index is an ele-
                // ment in the follow set of the current non-terminal.

                // TODO: If p is an epsilon production
                if ( ??? )
                {
                    set = followSet.second;

                    for (string s : set)
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
}