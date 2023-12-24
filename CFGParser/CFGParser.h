#ifndef CFG_PARSER_H
#define CFG_PARSER_H

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
    static vector<pair<string, vector<string>>> parse_rule(const vector<pair<string, string>> &rule_lines);
    static vector<pair<string, set<vector<string>>>> get_maped_rules(const vector<pair<string, vector<string>>> &rules);
    static vector<pair<string, set<vector<string>>>> get_CFG_rules(const string &rules_file_path);
};

#endif // CFG_PARSER_H
