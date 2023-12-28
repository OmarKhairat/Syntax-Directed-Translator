#include "CFGParser/CFGParser.h"
#include <sstream>
#include "FirstFollow/First_Follow.h"
#include "ParsingTable/ParsingTable.h"
#include "TopDownParser/TopDownParser.h"
#include "LexicalAnalyzer\LexicalAnalyzer.h"

using namespace std;

int main()
{
    // string projectPath = R"(D:\E\Collage\Year_4_1\Compilers\Project\Syntax-Directed-Translator\)";
    string projectPath = R"(D:\Development Workshop\Syntax-Directed-Translator\)";

    // Test CFGParser
    vector<pair<string, set<vector<string>>>> rules_map_set = CFGParser::get_CFG_rules(projectPath + "CFG_Rules.txt");

    // get keys of the map
    set<string> non_terminals;
    vector<string> NTs;

    for (const auto &entry : rules_map_set)
    {
        non_terminals.insert(entry.first);
        NTs.push_back(entry.first);
    }

    // print rules_map
    cout << "========= RULES after parsing, left recursion elemenation and left factoring =========" << endl;
    for (auto &rule : rules_map_set)
    {
        cout << rule.first << " -> ";
        int i = 0;
        for (auto &rhs : rule.second)
        {
            for (auto &rhs_part : rhs)
            {
                cout << rhs_part << " ";
            }
            if (i != rule.second.size() - 1)
                cout << endl
                     << "\t| ";
            else
            {
                cout << endl;
            }
            i++;
        }
    }

    First_Follow firstFollowObject;
    std::unordered_map<std::string, std::vector<std::string>> First = firstFollowObject.constructFirst(rules_map_set, non_terminals);
    std::vector<std::pair<std::string, std::vector<std::string>>> first;
    for (const auto &rule : rules_map_set)
    {
        // Extract key
        std::string key = rule.first;

        // Find the key in First unordered_map
        auto it = First.find(key);

        // Check if key exists in First
        if (it != First.end())
        {
            // Add a new pair to the 'first' vector
            first.emplace_back(key, it->second);
        }
    }
    std::unordered_map<std::string, std::vector<std::string>> Follow = firstFollowObject.constructFollow(rules_map_set, First, non_terminals);
    std::vector<std::pair<std::string, std::vector<std::string>>> follow;

    for (const auto &rule : rules_map_set)
    {
        // Extract key
        std::string key = rule.first;

        // Find the key in First unordered_map
        auto it = Follow.find(key);

        // Check if key exists in First
        if (it != Follow.end())
        {
            // Add a new pair to the 'first' vector
            follow.emplace_back(key, it->second);
        }
    }

    cout << endl << "=== FIRST ===" << endl;
    for (const auto &entry : first)
    {
        std::cout << "Key: " << entry.first << ", Values: ";
        for (const auto &value : entry.second)
        {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    cout << endl << "=== FOLLOW ===" << endl;

    for (const auto &entry : follow)
    {
        std::cout << "Key: " << entry.first << ", Values: ";
        for (const auto &value : entry.second)
        {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    cout << endl;

    ParsingTable pt(rules_map_set, first, follow, NTs);

    unordered_map<string, unordered_map<string, vector<vector<string>>>> table = pt.getTable();

    vector<pair<string, unordered_map<string, vector<vector<string>>>>> sortedTable;

    for (const auto &rule : rules_map_set)
    {
        // Extract key
        string key = rule.first;

        // Find the key in the table
        auto it = table.find(key);

        // Check if key exists in First
        if (it != table.end())
        {
            // Add a new pair to the 'first' vector
            sortedTable.emplace_back(*it);
        }
    }

    cout << endl;

    // Check the ambiguity of the sortedTable.
    for (const auto &outerPair : sortedTable)
    {
        for (const auto &innerPair : outerPair.second)
        {
            if (innerPair.second.size() > 1)
            {
                cout << "ENCOUNTERED AMBIGUITY! EXITING..." << endl;
                exit(-1);
            }
        }
    }

    // Print the elements of the sortedTable
    cout << endl << "=== PARSING TABLE ===" << endl;
    for (const auto &outerPair : sortedTable)
    {
        cout << "Key: " << outerPair.first << endl;

        for (const auto &innerPair : outerPair.second)
        {
            cout << "  Token: " << innerPair.first << endl;

            for (const auto &innerVector : innerPair.second)
            {
                cout << "    Productions: ";

                for (const auto &str : innerVector)
                {
                    cout << str << " ";
                }
                cout << endl;
            }
        }
    }

    /* Preparing the inputs for the top down parser. */
    auto it = find(NTs.begin(), NTs.end(), R"(\L)");

    if (it != NTs.end())
    {
        NTs.erase(it);
    }

    cout << endl;
    LexicalAnalyzerFactory factory(projectPath);
    LexicalAnalyzer lexicalAnalyzer = factory.getLexicalAnalyzer();
    cout << endl;

    string testProgram = "int x;\n"
                         "x = 5;\n"
                         "if (x > 2)\n"
                         "{\n"
                         "\t x = 0;\n"
                         "}\n";

    lexicalAnalyzer.setExpression(testProgram);

    TopDownParser tdp(table, NTs, lexicalAnalyzer);

    vector<string> output = tdp.parse();

    return 0;
}
