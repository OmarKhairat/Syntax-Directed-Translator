#ifndef CFG_PARSER_H
#define CFG_PARSER_H

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <set>

using namespace std;

class CFGParser
{
public:
    static vector<pair<string, string>> get_rules_lines(const string &rules_file_path);
    static unordered_map<string, vector<string>> parse_rule(const vector<pair<string, string>> &rule_lines);
    static unordered_map<string, set<vector<string>>> get_maped_rules(const unordered_map<string, vector<string>> &rules);
    static unordered_map<string, set<vector<string>>> get_CFG_rules(const string &rules_file_path);
};

#endif // CFG_PARSER_H
