#ifndef SDT_PARSER_H
#define SDT_PARSER_H
#include <bits/stdc++.h>
#include <unordered_map>

using namespace std;

class Parser{
    public:
        vector< string > get_rules_lines(string rules_file_path);
        vector< pair< string, string > > get_regular_def_lines(vector<string> rules);
        vector< string > get_punctuation_lines(vector<string> rules);
        vector< pair<string, string> > get_regular_expr_lines(vector<string> rules);
        vector< string > get_keywords_lines(vector<string> rules);
        vector< string > parse_keywords(vector<string> keywords_lines);
        vector< string > parse_punctuation(vector<string> punctuation_lines);
        unordered_map< string, vector<string> > parse_defs(vector< pair<string, string> > def_lines);
        vector< pair< string, vector<string> > > parse_expr(vector< pair<string, string> > expr_lines, unordered_map< string, vector<string> > defs);
        vector< pair< string, vector<string> > > convert_exprs_to_pos(vector< pair< string, vector<string> > > exprs);
        vector< string > parse_rhs(string rhs_line);
        string remove_pre_spaces(string s);
        string eliminate_back_slashes(string s);
        bool is_special_char(char c);
        string modifiy_dot_token(string s);
        vector<string> replace_dashes(vector<string> parsed_tokens);
        vector<string> infixtoPos(vector<string> infix);
};

#endif //SDT_PARSER_H

