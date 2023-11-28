#include "parser.h"
#include <unordered_map>
#include <bits/stdc++.h>

using namespace std;

vector< string > Parser::get_rules_lines(string rules_file_path)
{
    ifstream file(rules_file_path);
    vector<string> rules;
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file." << endl;
        return rules; 
    }   
    string rule;
    while (getline(file, rule)) {
        rules.push_back(rule);
    }
    file.close();
    return rules;
}

vector<string> Parser::get_keywords_lines(vector<string> rules)
{
    vector<string> keywords;
    for(const auto& rule : rules)
    {
        if(rule[0] == '{' && rule[rule.length() - 1] == '}')
        {
            keywords.push_back(rule);
        }
    }
    return keywords;
}
vector<string> Parser::get_punctuation_lines(vector<string> rules)
{
    vector<string> punctuations;
    for(const auto& rule : rules)
    {
        if(rule[0] == '[' && rule[rule.length() - 1] == ']')
        {
            punctuations.push_back(rule);
        }
    }
    return punctuations;
}

vector< pair<string, string> > Parser::get_regular_def_lines(vector<string> rules)
{
    vector< pair<string, string> > defs;
    for(const auto& rule : rules)
    {
        size_t eqIdx = rule.find('=');
        if (eqIdx != string::npos && eqIdx != 0 && eqIdx != rule.length() && rule[eqIdx - 1] == ' ' && rule[eqIdx + 1] == ' ')
        {
            size_t colonIdx = rule.find(':');
            if(colonIdx != string::npos && eqIdx > colonIdx)
                continue;
            string LHS = rule.substr(0, eqIdx - 1);
            string RHS = rule.substr(eqIdx + 2);
            defs.push_back(make_pair(LHS, RHS));
        }
    }
    return defs;
}
vector< pair<string, string> > Parser::get_regular_expr_lines(vector<string> rules)
{
    vector< pair<string, string> > exprs;
    for(const auto& rule : rules)
    {
        size_t colonIdx = rule.find(':');
        if (colonIdx != string::npos && colonIdx != 0 && colonIdx != rule.length() && rule[colonIdx + 1] == ' ' && rule[colonIdx] != '\\')
        {
            size_t eqIdx = rule.find('=');
            if(eqIdx != string::npos && colonIdx > eqIdx)
                continue;
            string LHS = rule.substr(0, colonIdx);
            string RHS = rule.substr(colonIdx + 2);
            exprs.push_back(make_pair(LHS, RHS));
        }
    }
    return exprs;
}
string Parser::remove_pre_spaces(string s)
{
    int i = 0;
    while(s[i] == ' ')
    {
        i++;
    }
    return s.substr(i);
}
string Parser::eliminate_back_slashes(string s)
{
    string res = "";
    int i = 0;
    while(i < s.length())
    {
        if(s[i] == '\\')
        {
            if(i + 1 < s.length() && (s[i + 1] == '\\' || s[i + 1] == 'L'))
            {
                string added_str = (s[i + 1] == '\\')? "\\" : "\\L";
                res += added_str;
                i++;
            }
            i++;
            continue;
        }
        res += s[i];
        i++;
    }
    return res;
}
vector<string> Parser::parse_keywords(vector<string> keywords_lines)
{
    vector<string> keywords;
    string temp = "";
    for(string line : keywords_lines)
    {
        line = line.substr(1, line.length() - 2) + " ";
        for(int i = 0; i < line.length(); i++)
        {
            if(line[i] == ' ')
            {
                if(temp.compare("") != 0)
                {
                    keywords.push_back(eliminate_back_slashes(temp));
                }
                temp = "";
            }
            else
            {
                temp += line[i];
            }
        }
    }
    return keywords;
}
vector< string > Parser::parse_punctuation(vector<string> punctuation_lines)
{
    return parse_keywords(punctuation_lines);
}

bool Parser::is_special_char(char c)
{
    char special_char[] = {'+', '-', '|', '(', ')', '*'};
    int n = sizeof(special_char) / sizeof(special_char[0]);
    bool res = false;
    for(int i = 0; i < n && !res; i++)
    {
        res = (c == special_char[i]);
    }
    return res;
}

string Parser::modifiy_dot_token(string s)
{
    if(s.compare(".") != 0)
    {
        return s;
    }
    return "\\.";
}

vector<string> Parser::replace_dashes(vector<string> parsed_tokens)
{
    vector<string> res;
    string dummy = "";
    for(int i = 0; i < parsed_tokens.size(); i++)
    {
        if(parsed_tokens[i].compare("-") == 0)
        {
            string token_l = res.back();
            res.pop_back();
            string token_r = parsed_tokens[i + 1];
            res.push_back(token_l[0] + dummy);
            for(char c = token_l[0] + 1; c <= token_r[0]; ++c)
            {
                res.push_back("|");
                res.push_back(c + dummy);
            }
            i++;
            continue;
        }
        res.push_back(parsed_tokens[i]);
    }
    return res;
}

vector< string > Parser::parse_rhs(string rhs_line)
{
    vector<string> parsed_tokens;
    int i = 0;
    string temp = "";
    string dummy = "";
    rhs_line = " " + rhs_line + " ";
    bool char_found = false;
    // cout << "\t\trhs_line:" << rhs_line << endl;
    while(i < rhs_line.length())
    {
        if(is_special_char(rhs_line[i]) && i != 0 && rhs_line[i - 1] != '\\')
        {
            char_found = true;
            if(temp.compare("") != 0)
            {
                parsed_tokens.push_back(modifiy_dot_token(temp));
                if(rhs_line[i] == '(' )
                {
                    parsed_tokens.push_back(".");
                }
                temp = "";
            }
            parsed_tokens.push_back(rhs_line[i] + dummy);
        }
        else if(rhs_line[i] == ' ' && char_found && !parsed_tokens.empty())
        {
            string last_temp = parsed_tokens.back();
            if(i == rhs_line.length() - 1)
            {
                if(temp.compare("") != 0)
                {
                    parsed_tokens.push_back(modifiy_dot_token(temp));
                }
            }
            else if(rhs_line[i + 1] != ' ')
            {
                if(temp.compare("") != 0)
                {
                    parsed_tokens.push_back(modifiy_dot_token(temp));
                }
                if(((last_temp.compare("|") != 0 && last_temp.compare("-") != 0 && last_temp.compare("(") != 0) || temp.compare("") != 0)
                    && rhs_line[i + 1] != '|' && rhs_line[i + 1] != '-' && rhs_line[i + 1] != '+' 
                    && rhs_line[i + 1] != '*' && rhs_line[i + 1] != ')')
                {
                    parsed_tokens.push_back(".");
                }
            }
            temp = "";
        }
        else if(i == rhs_line.length() - 1 && rhs_line[i] == ' ' && parsed_tokens.empty() && temp.compare("") != 0)
        {
            parsed_tokens.push_back(modifiy_dot_token(temp));
        }
        else if(rhs_line[i] != ' ')
        {
            char_found = true;
            temp += rhs_line[i];
        }
        i++;
    }
    return replace_dashes(parsed_tokens);
}

unordered_map< string, vector<string> > Parser::parse_defs(vector< pair<string, string> > def_lines)
{
    unordered_map< string, vector<string> > defs;
    for(int i = 0; i < def_lines.size(); i++)
    {
        string lhs = def_lines[i].first;
        string rhs_line = def_lines[i].second;
        vector<string> temp_rhs = parse_rhs(rhs_line);
        vector<string> rhs;
        for(int j = 0; j < temp_rhs.size(); j++)
        {
            auto it = defs.find(temp_rhs[j]);
            if(it != defs.end())
            {
                vector<string> v = defs[temp_rhs[j]];
                v.insert(v.begin(), "(");
                v.push_back(")");
                rhs.insert(rhs.end(), v.begin(), v.end());
            }
            else
            {
                rhs.push_back(temp_rhs[j]);
            }
        }
        defs[lhs] = rhs;
    }
    return defs;
}

vector< pair< string, vector<string> > > Parser::parse_expr(vector< pair<string, string> > expr_lines, unordered_map< string, vector<string> > defs)
{
    vector< pair< string, vector<string> > > exprs;
    for(int i = 0; i < expr_lines.size(); i++)
    {
        string lhs = expr_lines[i].first;
        string rhs_line = expr_lines[i].second;
        vector<string> temp_rhs = parse_rhs(rhs_line);
        vector<string> rhs;
        for(int j = 0; j < temp_rhs.size(); j++)
        {
            auto it = defs.find(temp_rhs[j]);
            if(it != defs.end())
            {
                vector<string> v = defs[temp_rhs[j]];
                v.insert(v.begin(), "(");
                v.push_back(")");
                rhs.insert(rhs.end(), v.begin(), v.end());
            }
            else
            {
                rhs.push_back(temp_rhs[j]);
            }
        }
        exprs.push_back(make_pair(lhs, rhs));
    }
    return exprs;
}

int main()
{
    // cout << ("|" + "") << endl;
    Parser p;
    vector<string> rules = p.get_rules_lines("/Users/omarkhairat/Documents/GitHub/Syntax-Directed-Translator/lexical_rules.txt");
    // for(string rule : rules)
    //     cout << rule << endl;
    // cout << "******************************************************************" << endl;

    vector<string> keywords_lines = p.get_keywords_lines(rules);
    vector<string> keywords = p.parse_keywords(keywords_lines);
    // for(string keyword : keywords)
    // {
    //     cout << "*" <<keyword << "*" << endl;
    // }
    // cout << "******************************************************************" << endl;

    vector<string> punctuations_lines = p.get_punctuation_lines(rules);
    vector<string> punctuations = p.parse_keywords(punctuations_lines);
    // for(string punctuation : punctuations)
    //     cout << "*" <<punctuation << "*" << endl;
    // cout << "******************************************************************" << endl;

    
    // vector<string> v = p.parse_rhs("   digits     ");
    // for(string tok : v)
    //     cout << "*" <<tok << "*" << endl;


    vector< pair<string, string> > defs_lines = p.get_regular_def_lines(rules);
    unordered_map< string, vector<string> > defs = p.parse_defs(defs_lines);
    // for (const auto& pair : defs) {
    //     std::cout << "key: " << pair.first << std::endl << "\t";
    //     for(string tok : pair.second)
    //         cout <<tok << " ";
    //     cout << endl;
    // }

    vector< pair<string, string> > expr_lines = p.get_regular_expr_lines(rules);
    vector< pair< string, vector<string> > > exprs = p.parse_expr(expr_lines, defs);
    // for (const auto& pair : exprs) {
    //     std::cout << "key: " << pair.first << std::endl << "\t";
    //     for(string tok : pair.second)
    //         cout <<tok << " ";
    //     cout << endl;
    // }

    
    return 0;
}