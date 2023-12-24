#include "CFGParser.h"

using namespace std;

string remove_all_spaces(const string &str)
{
    string res = "";
    for (char c : str)
        if (c != ' ')
            res += c;
    
    return res;
}

vector<pair<string, string>> CFGParser::get_rules_lines(const string &rules_file_path)
{
    ifstream file(rules_file_path);
    vector<pair<string, string>> rules;
    if (!file.is_open())
    {
        cerr << "Error: Unable to open the file." << endl;
        return rules;
    }

    string LHS = "", RHS = "", ruleLine = "";
    while (getline(file, ruleLine))
    {
        if(ruleLine.at(0) == '#'){  // new rule
            if(!LHS.empty() && !RHS.empty())
                rules.emplace_back(remove_all_spaces(LHS), RHS);
            size_t pos = ruleLine.find("=");
            if (pos != string::npos){   //found
                LHS = ruleLine.substr(1, pos-1);
                RHS = ruleLine.substr(pos + 1);
            }else{                      // not found all that in the LHS
                LHS = ruleLine.substr(1);
                RHS = "" ;
            }
        }else{                      // not start of rule
            size_t pos = ruleLine.find("=");
            if (pos != string::npos){   //found that the end of LHS
                LHS.append(ruleLine.substr(0, pos));
                RHS = ruleLine.substr(pos + 1);
            }else{                      // not found all that in the RHS
                RHS.append(ruleLine) ;
            }   
        }
    }
    if(!LHS.empty() && !RHS.empty())
        rules.emplace_back(remove_all_spaces(LHS), RHS);
    file.close();
    return rules;
}

bool is_special_char(char c)
{
    char special_char[] = {' ', '\n', '\t', '\'', '|', '\\', '+', '-', '(', ')', '*'};
    int n = sizeof(special_char) / sizeof(special_char[0]);
    bool res = false;
    for (int i = 0; i < n && !res; i++)
    {
        res = (c == special_char[i]);
    }
    return res;
}

vector<pair<string, vector<string>>> CFGParser::parse_rule(const vector<pair<string, string>> &rule_lines){
    vector<pair<string, vector<string>>> rules ;
    for(auto &rule: rule_lines){
        string LHS = rule.first;
        string RHS = rule.second;
        vector<string> RHS_vector;
        // split by  " ", "|", "+", "*", "(", ")", "'" unless it between ''
        size_t startToken=0, endToken=0 ;
        while(endToken < RHS.length()){
            if(is_special_char(RHS.at(endToken))){
                if(RHS.at(endToken) == ' ' || RHS.at(endToken) == '\n' || RHS.at(endToken) == '\t'){
                    if(startToken!=endToken){       // Non-terminal                      
                        RHS_vector.push_back(RHS.substr(startToken, endToken-startToken));
                    }                               // else is just space
                }
                else if(RHS.at(endToken) == '\''){  // Terminal
                    if(startToken!=endToken){                    
                        // add the prev token if existed
                        RHS_vector.push_back(RHS.substr(startToken, endToken-startToken));
                    }
                    // find the next '
                    startToken = endToken+1;
                    endToken = RHS.find("'", startToken);
                    if (endToken != string::npos){   //found
                        RHS_vector.push_back(RHS.substr(startToken, endToken-startToken));
                    }else{
                        //error in rules
                        return vector<pair<string, vector<string>>>();
                    }
                }
                else if(RHS.at(endToken) == '\\' && endToken+1< RHS.length() && RHS.at(endToken+1) == 'L'){
                    // epsilon
                    RHS_vector.push_back("\\L");
                    endToken ++;
                }
                else{
                    // +, *, (, ), |
                    if(startToken!=endToken){                    
                        // add the prev token if existed
                        RHS_vector.push_back(RHS.substr(startToken, endToken-startToken));
                    }
                    RHS_vector.push_back(RHS.substr(endToken, 1));
                }
                endToken ++;
                startToken = endToken;
            }else{
                endToken ++;
            }
        }
        if(startToken!=endToken){                    
            // add the prev token if existed
            RHS_vector.push_back(RHS.substr(startToken, endToken-startToken));
        }
        rules.push_back(make_pair(LHS, RHS_vector));
    }
    return rules;
}

vector<pair<string, set<vector<string>>>> CFGParser::get_maped_rules(const vector<pair<string, vector<string>>> &rules){
    vector<pair<string, set<vector<string>>>> maped_rules;
    for(auto &rule: rules){
        string LHS = rule.first;
        vector<string> RHS = rule.second;
        set<vector<string>> RHS_set ;
        size_t start = 0;
        for(size_t end = 0; end < RHS.size(); end++){
            if(RHS.at(end) == "|"){
                if(start != end){
                    RHS_set.insert(vector<string>(RHS.begin()+start, RHS.begin()+end));
                }
                start = end+1;
            }
        }
        if(start != RHS.size()){
            RHS_set.insert(vector<string>(RHS.begin()+start, RHS.end()));
            
        }
        maped_rules.push_back(make_pair(LHS, RHS_set)) ;
    }
    return maped_rules;
}

vector<pair<string, set<vector<string>>>> CFGParser::get_CFG_rules(const string &rules_file_path){
    vector<pair<string, string>> rules = get_rules_lines(rules_file_path);
    vector<pair<string, vector<string>>> rules_map = parse_rule(rules);
    return get_maped_rules(rules_map);
}
