#ifndef CFG_PARSER_H
#define CFG_PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <set>
#include <unordered_map>
#include <queue>

using namespace std;

class CFGParser
{
private:
    static unordered_map<string, string> get_rules_lines(const string &rules_file_path);
    static unordered_map<string, vector<string>> parse_rule(const unordered_map<string, string> &rule_lines);
    static unordered_map<string, set<vector<string>>> get_maped_rules(const unordered_map<string, vector<string>> &rules);
    static void topologicalSortUtil(const string &nonterminal, const unordered_map<string, set<vector<string>>> &rulesMap, 
                        set<string> &visited, stack<string> &s, set<string> &nonterminals);
    static vector<pair<string, set<vector<string>>>> topologicalSort(const unordered_map<string, set<vector<string>>> &rulesMap);
    static unordered_map<string, set<vector<string>>> eleminateLeftRecursion(const unordered_map<string, set<vector<string>>> &rulesMap);
    static unordered_map<string, set<vector<string>>> LeftFactoring(const unordered_map<string, set<vector<string>>> &rulesMap);
public:
    static vector<pair<string, set<vector<string>>>> get_CFG_rules(const string &rules_file_path);
};

#endif // CFG_PARSER_H
