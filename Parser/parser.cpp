#include "parser.h"
#include <iostream>
#include <fstream>

using namespace std;

// -------------------Parser-------------------
vector<string> Parser::get_rules_lines(const string &rules_file_path) {
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

vector<string> Parser::get_keywords_lines(const vector<string> &rules) {
    vector<string> keywords;
    for (const auto &rule: rules) {
        if (rule[0] == '{' && rule[rule.length() - 1] == '}') {
            keywords.push_back(rule);
        }
    }
    return keywords;
}

vector<string> Parser::get_punctuation_lines(const vector<string> &rules) {
    vector<string> punctuations;
    for (const auto &rule: rules) {
        if (rule[0] == '[' && rule[rule.length() - 1] == ']') {
            punctuations.push_back(rule);
        }
    }
    return punctuations;
}

vector<pair<string, string> > Parser::get_regular_def_lines(const vector<string> &rules) {
    vector<pair<string, string> > defs;
    for (const auto &rule: rules) {
        size_t eqIdx = rule.find('=');
        if (eqIdx != string::npos && eqIdx != 0 && eqIdx != rule.length() && rule[eqIdx - 1] == ' ' &&
            rule[eqIdx + 1] == ' ') {
            size_t colonIdx = rule.find(':');
            if(colonIdx != string::npos && eqIdx > colonIdx)
                continue;
            string LHS = rule.substr(0, eqIdx - 1);
            string RHS = rule.substr(eqIdx + 2);
            defs.emplace_back(LHS, RHS);
        }
    }
    return defs;
}

vector<pair<string, string> > Parser::get_regular_expr_lines(const vector<string> &rules) {
    vector<pair<string, string> > exprs;
    for (const auto &rule: rules) {
        size_t colonIdx = rule.find(':');
        if (colonIdx != string::npos && colonIdx != 0 && colonIdx != rule.length() && rule[colonIdx + 1] == ' ' &&
            rule[colonIdx] != '\\') {
            size_t eqIdx = rule.find('=');
            if(eqIdx != string::npos && colonIdx > eqIdx)
                continue;
            string LHS = rule.substr(0, colonIdx);
            string RHS = rule.substr(colonIdx + 2);
            exprs.emplace_back(LHS, RHS);
        }
    }
    return exprs;
}

string Parser::remove_pre_spaces(string s) {
    int i = 0;
    while (s[i] == ' ') {
        i++;
    }
    return s.substr(i);
}

string Parser::eliminate_back_slashes(string s) {
    string res;
    int i = 0;
    while (i < s.length()) {
        if (s[i] == '\\') {
            if (i + 1 < s.length() && (s[i + 1] == '\\' || s[i + 1] == 'L')) {
                string added_str = (s[i + 1] == '\\') ? "\\" : "\\L";
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

vector<string> Parser::parse_keywords(const vector<string> &keywords_lines) {
    vector<string> keywords;
    string temp;
    for (string line: keywords_lines) {
        line = line.substr(1, line.length() - 2) + " ";
        for (char i: line) {
            if (i == ' ') {
                if (!temp.empty()) {
                    keywords.push_back(eliminate_back_slashes(temp));
                }
                temp = "";
            } else {
                temp += i;
            }
        }
    }
    return keywords;
}

vector<string> Parser::parse_punctuation(const vector<string> &punctuation_lines) {
    return parse_keywords(punctuation_lines);
}

bool Parser::is_special_char(char c) {
    char special_char[] = {'+', '-', '|', '(', ')', '*'};
    int n = sizeof(special_char) / sizeof(special_char[0]);
    bool res = false;
    for (int i = 0; i < n && !res; i++) {
        res = (c == special_char[i]);
    }
    return res;
}

string Parser::modifiy_dot_token(string s) {
    if (s != ".") {
        return s;
    }
    return "\\.";
}

vector<string> Parser::replace_dashes(vector<string> parsed_tokens) {
    vector<string> res;
    string dummy;
    for (int i = 0; i < parsed_tokens.size(); i++) {
        if (parsed_tokens[i] == "-") {
            string token_l = res.back();
            res.pop_back();
            string token_r = parsed_tokens[i + 1];
            res.push_back(token_l[0] + dummy);
            for (int c = token_l[0] + 1; c <= token_r[0]; ++c) {
                res.emplace_back("|");
                res.push_back(char(c) + dummy);
            }
            i++;
            continue;
        }
        res.push_back(parsed_tokens[i]);
    }
    return res;
}

vector<string> Parser::parse_rhs(string rhs_line) {
    vector<string> parsed_tokens;
    int i = 0;
    string temp;
    string dummy;
    rhs_line = " " + rhs_line + " ";
    bool char_found = false;
    while (i < rhs_line.length()) {
        if (is_special_char(rhs_line[i]) && i != 0 && rhs_line[i - 1] != '\\') {
            char_found = true;
            if (!temp.empty()) {
                parsed_tokens.push_back(modifiy_dot_token(temp));
                if (rhs_line[i] == '(') {
                    parsed_tokens.emplace_back(".");
                }
                temp = "";
            }
            parsed_tokens.push_back(rhs_line[i] + dummy);
        } else if (rhs_line[i] == ' ' && char_found && !parsed_tokens.empty()) {
            string last_temp = parsed_tokens.back();
            if (i == rhs_line.length() - 1) {
                if (!temp.empty()) {
                    parsed_tokens.push_back(modifiy_dot_token(temp));
                }
            } else if (rhs_line[i + 1] != ' ') {
                if (!temp.empty()) {
                    parsed_tokens.push_back(modifiy_dot_token(temp));
                }
                if (((last_temp != "|" && last_temp != "-" && last_temp != "(") || !temp.empty())

                    && rhs_line[i + 1] != '|' && rhs_line[i + 1] != '-' && rhs_line[i + 1] != '+'

                    && rhs_line[i + 1] != '*' && rhs_line[i + 1] != ')') {
                    parsed_tokens.emplace_back(".");
                }
            }
            temp = "";
        } else if (i == rhs_line.length() - 1 && rhs_line[i] == ' ' && parsed_tokens.empty() && !temp.empty()) {
            parsed_tokens.push_back(modifiy_dot_token(temp));
        } else if (rhs_line[i] != ' ') {
            char_found = true;
            temp += rhs_line[i];
        }
        i++;
    }
    return replace_dashes(parsed_tokens);
}

unordered_map<string, vector<string> > Parser::parse_defs(const vector<pair<string, string> > &def_lines) {
    unordered_map<string, vector<string> > defs;
    for (auto &def_line: def_lines) {
        string lhs = def_line.first;
        string rhs_line = def_line.second;
        vector<string> temp_rhs = parse_rhs(rhs_line);
        vector<string> rhs;
        for (const auto &temp_rh: temp_rhs) {
            auto it = defs.find(temp_rh);
            if (it != defs.end()) {
                vector<string> v = defs[temp_rh];
                v.insert(v.begin(), "(");
                v.emplace_back(")");
                rhs.insert(rhs.end(), v.begin(), v.end());
            } else {
                rhs.push_back(temp_rh);
            }
        }
        defs[lhs] = rhs;
    }
    return defs;
}

vector<pair<string, vector<string> > >
Parser::parse_expr(const vector<pair<string, string> > &expr_lines, unordered_map<string, vector<string> > defs) {
    vector<pair<string, vector<string> > > exprs;
    for (auto &expr_line: expr_lines) {
        string lhs = expr_line.first;
        string rhs_line = expr_line.second;
        vector<string> temp_rhs = parse_rhs(rhs_line);
        vector<string> rhs;
        for (const auto &temp_rh: temp_rhs) {
            auto it = defs.find(temp_rh);
            if (it != defs.end()) {
                vector<string> v = defs[temp_rh];
                v.insert(v.begin(), "(");
                v.emplace_back(")");
                rhs.insert(rhs.end(), v.begin(), v.end());
            } else {
                rhs.push_back(temp_rh);
            }
        }
        exprs.emplace_back(lhs, rhs);
    }
    return exprs;
}

vector<string> Parser::infixtoPos(const vector<string> &infix) {
    vector<string> pos;
    stack<string> stck;
    unordered_map<string, int> special_chars;
    special_chars["*"] = 5; special_chars["+"] = 4;
    special_chars["."] = 3; special_chars["|"] = 2;
    special_chars["("] = special_chars[")"] = 0;
    for (const string &token: infix) {
        if (token == "(") {
            stck.push(token);
        } else if (token == ")") {
            while (!stck.empty() && stck.top() != "(") {
                pos.push_back(stck.top());
                stck.pop();
            }
            if(!stck.empty())
                stck.pop();
        } else if (token == "*" || token == "+" || token == "|" || token == ".") {
            while (!stck.empty() && special_chars[token] <= special_chars[stck.top()]) {
                pos.push_back(stck.top());
                stck.pop();
            }
            stck.push(token);
        } else {
            pos.push_back(token);
        }
    }
    while (!stck.empty()) {
        pos.push_back(stck.top());
        stck.pop();
    }
    return pos;
}

vector<pair<string, vector<string> > >
Parser::convert_exprs_to_pos(const vector<pair<string, vector<string> > > &exprs) {
    vector<pair<string, vector<string> > > res;
    for (const pair<string, vector<string> > &expr: exprs) {
        res.emplace_back(expr.first, infixtoPos(expr.second));
    }
    return res;
}
