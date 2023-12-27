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
    static unordered_map<string, vector<string>> split_RHS(const unordered_map<string, string> &rule_lines);
    static unordered_map<string, set<vector<string>>> split_by_or(const unordered_map<string, vector<string>> &rules);
    static void topological_sort_util(const string &nonterminal, const unordered_map<string, set<vector<string>>> &rulesMap,
                                      set<string> &visited, stack<string> &s, set<string> &nonterminals);
    static vector<pair<string, set<vector<string>>>> topological_sort(const unordered_map<string, set<vector<string>>> &rulesMap);
    static unordered_map<string, set<vector<string>>> eleminate_left_rec(const unordered_map<string, set<vector<string>>> &rulesMap);
    static unordered_map<string, set<vector<string>>> Left_fact(const unordered_map<string, set<vector<string>>> &rulesMap);

public:
    static vector<pair<string, set<vector<string>>>> get_CFG_rules(const string &rules_file_path);
};

#endif // CFG_PARSER_H
