#ifndef SDT_PARSER_H
#define SDT_PARSER_H

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

class Parser
{
public:
    struct DFA_State;

    class NFA;

    static vector<string> get_rules_lines(const string &rules_file_path);

    static vector<pair<string, string>> get_regular_def_lines(const vector<string> &rules);

    static vector<string> get_punctuation_lines(const vector<string> &rules);

    static vector<pair<string, string>> get_regular_expr_lines(const vector<string> &rules);

    static vector<string> get_keywords_lines(const vector<string> &rules);

    static vector<string> parse_keywords(const vector<string> &keywords_lines);

    static vector<string> parse_punctuation(const vector<string> &punctuation_lines);

    static unordered_map<string, vector<string>> parse_defs(const vector<pair<string, string>> &def_lines);

    static vector<pair<string, vector<string>>>
    parse_expr(const vector<pair<string, string>> &expr_lines, unordered_map<string, vector<string>> defs);

    static vector<pair<string, vector<string>>>
    convert_exprs_to_pos(const vector<pair<string, vector<string>>> &exprs);

    static vector<string> parse_rhs(string rhs_line);

    static string remove_pre_spaces(string s);

    static string eliminate_back_slashes(string s);

    static bool is_special_char(char c);

    static string modifiy_dot_token(string s);

    static vector<string> replace_dashes(vector<string> parsed_tokens);

    static vector<string> infixtoPos(const vector<string> &infix);
};

#endif // SDT_PARSER_H
