#include "parser.h"
#include <unordered_map>
#include <stack>
#include <set>
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// -------------------NFA-------------------
struct NFA_State
{
    int id;
    bool is_acceptance;
    string token; // if is_acceptance is true
    unordered_map< string, set<int> > transitions;
};

struct DFA_State
{
    set<int> nfa_states;
    bool is_acceptance;
    string token;
    unordered_map< string, set<int> > transitions;
};

class NFA {
public:
    NFA();
    void addState(int id, bool is_acceptance = false, bool is_start = false , string token = "");
    void addTransition(int from, string symbol, int to);
    NFA pop();
    void push(NFA nfa);
    void concatenate();
    void concatenateAllStack();
    void orOp();
    void kleeneStar();
    void positiveClosure();
    void processSymbol(string symbol);
    unordered_map< int, set<int> > getEpsilonClosureSetMap();
    void toJSON(string file_path);
    unordered_map< int, NFA_State > states;
    vector<int> start_states;
    vector<int> end_states;

private:
    stack<NFA> nfa_stack;
    int stateCounter;
};

NFA::NFA()
{
    stateCounter = 0;
}

void NFA::addState(int id, bool is_acceptance, bool is_start, string token)
{
    NFA_State state;
    state.id = id;
    state.is_acceptance = is_acceptance;
    if (is_acceptance)
        end_states.push_back(id);
    if (is_start)
        start_states.push_back(id);
    state.token = token;
    states[id] = state;
}

void NFA::addTransition(int from, string symbol, int to)
{
    set<int> to_states = states[from].transitions[symbol];
    to_states.insert(to);
    states[from].transitions[symbol] = to_states;
}

NFA NFA::pop()
{
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();
    return nfa;
}

void NFA::push(NFA nfa)
{
    nfa_stack.push(nfa);
}

void NFA::concatenate()
{
    NFA nfa2 = nfa_stack.top();
    nfa_stack.pop();
    NFA nfa1 = nfa_stack.top();
    nfa_stack.pop();

    for(auto state : nfa2.states) // copy states of nfa2 to nfa1
    {
        nfa1.states[state.first] = state.second;
    }
    for (int state_i : nfa1.end_states)
    {
        NFA_State state = nfa1.states[state_i];
        state.is_acceptance = false;
        nfa1.states[state_i] = state;
        for (int state_j : nfa2.start_states)
        {
            nfa1.addTransition(state_i, "\\L", state_j);
        }
    }
    nfa1.end_states = nfa2.end_states;
    nfa_stack.push(nfa1);
}

void NFA::concatenateAllStack()
{
    if (nfa_stack.size() == 0)
        return;
    if (nfa_stack.size() == 1)
    {
        NFA nfa = nfa_stack.top();
        nfa_stack.pop();
        for (auto state : nfa.states)   // copy states
        {
            states[state.first] = state.second;
        }
        start_states = nfa.start_states;
        end_states = nfa.end_states;
        return;
    }

    start_states.clear();
    end_states.clear();
    states.clear();
    int new_start_state = stateCounter++;
    addState(new_start_state, false, true);

    while (nfa_stack.size() > 0)
    {
        NFA nfa = nfa_stack.top();
        nfa_stack.pop();

        for (auto state : nfa.states)   // copy states
        {
            states[state.first] = state.second;
        }
        for (int state_i : nfa.start_states)
        {
            addTransition(new_start_state, "\\L", state_i);
        }
        end_states.insert(end_states.end(), nfa.end_states.begin(), nfa.end_states.end());
    }

}

void NFA::orOp()
{
    // pop 2 NFAs
    NFA nfa2 = nfa_stack.top();
    nfa_stack.pop();
    NFA nfa1 = nfa_stack.top();
    nfa_stack.pop();

    // new start and end states and new NFA
    NFA new_nfa;
    int new_start_state = stateCounter++;
    int new_end_state = stateCounter++;
    new_nfa.addState(new_start_state, false, true);
    new_nfa.addState(new_end_state, true);

    // copy states
    for(auto state : nfa1.states)
    {
        new_nfa.states[state.first] = state.second;
    }
    for(auto state : nfa2.states)
    {
        new_nfa.states[state.first] = state.second;
    }

    // add transitions from the new start
    for (int state_i : nfa1.start_states)
    {
        new_nfa.addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i : nfa2.start_states)
    {
        new_nfa.addTransition(new_start_state, "\\L", state_i);
    }

    // add transitions to the new end
    for (int state_i : nfa1.end_states)
    {
        NFA_State state = new_nfa.states[state_i];
        state.is_acceptance = false;
        new_nfa.states[state_i] = state;
        new_nfa.addTransition(state_i, "\\L", new_end_state);
    }
    for (int state_i : nfa2.end_states)
    {
        NFA_State state = new_nfa.states[state_i];
        state.is_acceptance = false;
        new_nfa.states[state_i] = state;
        new_nfa.addTransition(state_i, "\\L", new_end_state);
    }

    nfa_stack.push(new_nfa);
}

void NFA::kleeneStar()
{
    // pop
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();

    // new start and end states and new NFA
    NFA new_nfa;
    int new_start_state = stateCounter++;
    int new_end_state = stateCounter++;
    new_nfa.addState(new_start_state, false, true);
    new_nfa.addState(new_end_state, true);

    for(auto state : nfa.states) // copy states
    {
        new_nfa.states[state.first] = state.second;
    }

    for (int state_i : nfa.start_states) // new start to old start
    {
        new_nfa.addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i : nfa.end_states) // old end to new end
    {
        NFA_State state = new_nfa.states[state_i];
        state.is_acceptance = false;
        new_nfa.states[state_i] = state;
        new_nfa.addTransition(state_i, "\\L", new_end_state);
    }
    for (int state_i : nfa.end_states) // old end to starts
    {
        for (int state_j : nfa.start_states)
        {
            new_nfa.addTransition(state_i, "\\L", state_j);
        }
    }
    new_nfa.addTransition(new_start_state, "\\L", new_end_state);

    nfa_stack.push(new_nfa);
}

void NFA::positiveClosure()
{
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();

    for (int state_i : nfa.end_states) // old end to starts
    {
        for (int state_j : nfa.start_states)
        {
            nfa.addTransition(state_i, "\\L", state_j);
        }
    }

    nfa_stack.push(nfa);
}

void NFA::processSymbol(string symbol)
{
    NFA new_nfa;
    int new_start_state = stateCounter++;
    new_nfa.addState(new_start_state, false, true);

    for(unsigned int i=0 ; i<symbol.length() ; i++)
    {
        if(i>0 && symbol[i-1] == '\\')
        {
            continue;
        }

        char c = symbol[i];
        int new_end_state = stateCounter++;
        new_nfa.addState(new_end_state);

        if(c == '\\')
        {
            new_nfa.addTransition(new_start_state, symbol.substr(i, 2), new_end_state);

        }else{
            new_nfa.addTransition(new_start_state, string(1, c), new_end_state);
        }
        new_start_state = new_end_state;
    }
    new_nfa.states[new_start_state].is_acceptance = true;
    new_nfa.end_states.push_back(new_start_state);

    nfa_stack.push(new_nfa);
}

unordered_map< int, set<int> > NFA::getEpsilonClosureSetMap()
{
    unordered_map<int, set<int>> epsilon_closure_set;
    for(auto state : states){
        int state_id = state.first;
        set<int> epsilon_closure_i;
        queue<int> q;
        q.push(state_id);
        while(!q.empty()){
            int current_state = q.front();
            q.pop();
            epsilon_closure_i.insert(current_state);
            for(int next_state : states[current_state].transitions["\\L"]){
                if(epsilon_closure_i.find(next_state) == epsilon_closure_i.end()){
                    if(epsilon_closure_set.find(next_state) != epsilon_closure_set.end()){
                        epsilon_closure_i.insert(epsilon_closure_set[next_state].begin(), epsilon_closure_set[next_state].end());
                    }else{
                        q.push(next_state);
                    }
                }
            }
        }
        epsilon_closure_set[state_id] = epsilon_closure_i;
    }
    return epsilon_closure_set;
}

void NFA::toJSON(string file_path)
{
    ofstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file." << endl;

        return; 
    }   
    file << "{" << endl;
    file << "\t\"start_states\": [" << endl;
    for(int i = 0 ; i < start_states.size() ; i++)
    {
        int start_state = start_states[i];
        file << "\t\t" << start_state ;
        if(i != start_states.size() - 1)
            file << "," ;
        file << endl;
    }
    file << "\t]," << endl;
    file << "\t\"end_states\": [" << endl;
    for(int i = 0 ; i < end_states.size() ; i++)
    {
        int start_state = end_states[i];
        file << "\t\t" << start_state ;
        if(i != end_states.size() - 1)
            file << "," ;
        file << endl;
    }
    file << "\t]," << endl;
    file << "\t\"states\": [" << endl;
    for(int i=0 ; i< states.size() ; i++)
    {
        NFA_State state = states[i];
        file << "\t\t{" << endl;
        file << "\t\t\t\"id\": " << i << "," << endl;
        file << "\t\t\t\"is_acceptance\": " << state.is_acceptance << "," << endl;
        file << "\t\t\t\"token\": \"" << state.token << "\"," << endl;
        file << "\t\t\t\"transitions\": [" << endl;
        int j = 0;
        for(auto transition : state.transitions)
        {
            file << "\t\t\t\t{" << endl;
            file << "\t\t\t\t\t\"symbol\": \"" << transition.first << "\"," << endl;
            file << "\t\t\t\t\t\"to\": [" << endl;
            unsigned k = 0;
            for(auto to_state : transition.second)
            {
                file << "\t\t\t\t\t\t" << to_state ;
                if(k != transition.second.size() - 1)
                    file << "," ;
                file << endl;
                k++;
            }
            file << "\t\t\t\t\t]" << endl;
            file << "\t\t\t\t}" ;
            if(j != state.transitions.size() - 1)
                file << ",";
            file << endl;
            j++ ;
        }
        file << "\t\t\t]" << endl;
        file << "\t\t}";
        if(i != states.size() - 1)
            file << "," ;
        file << endl;
    }
    file << "\t]" << endl;
    file << "}" << endl;
    file.close();
}


// -------------------Parser-------------------
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

vector<string> Parser::infixtoPos(vector<string> infix)
{
    vector<string> pos;
    stack<string> stck;
    unordered_map<string, int> special_chars;
    special_chars["*"] = 5; special_chars["+"] = 4;
    special_chars["."] = 3; special_chars["|"] = 2; 
    special_chars["("] = special_chars[")"] = 0;
    for(string token : infix)
    {
        if(token.compare("(") == 0)
        {
            stck.push(token);
        }
        else if(token.compare(")") == 0)
        {
            while(!stck.empty() && stck.top().compare("(") != 0)
            {
                pos.push_back(stck.top());
                stck.pop();
            }
            if(!stck.empty())
                stck.pop();
        }
        else if(token.compare("*") == 0 || token.compare("+") == 0 || token.compare("|") == 0 || token.compare(".") == 0)
        {
            while(!stck.empty() && special_chars[token] <= special_chars[stck.top()])
            {
                pos.push_back(stck.top());
                stck.pop();
            }
            stck.push(token);
        }
        else
        {
            pos.push_back(token);
        }
    }
    while(!stck.empty())
    {
        pos.push_back(stck.top());
        stck.pop();
    }
    return pos;
}

vector< pair< string, vector<string> > > Parser::convert_exprs_to_pos(vector< pair< string, vector<string> > > exprs)
{
    vector< pair< string, vector<string> > > res;
    for(pair< string, vector<string> > expr: exprs)
    {
        res.push_back(make_pair(expr.first, infixtoPos(expr.second)));
    }
    return res;
}

unordered_map<string, int> getPriority(vector< pair< string, vector<string> > > exprs, vector<string> keywords, vector<string> punctuations)
{
    unordered_map<string, int> priority;

    for(string keyword : keywords)
    {
        priority[keyword] = 0;
    }
    for(string punctuation : punctuations)
    {
        priority[punctuation] = 0;
    }
    for(int i = 0 ; i < exprs.size() ; i++)
    {
        priority[exprs[i].first] = i+1;
    }    
    return priority;
}

NFA convert_exprs_postfix_to_NFA(vector< pair< string, vector<string> > > exprs, vector<string> keywords, vector<string> punctuations, unordered_map<string, int> priority)
{
    NFA res;
    for(string keyword : keywords)
    {
        res.processSymbol(keyword);
        NFA expr_nfa = res.pop();
        for(int state_i : expr_nfa.end_states) // set token of acceptance states
        {
            expr_nfa.states[state_i].token = keyword;
        }
        res.push(expr_nfa);
    }
    for(string punctuation : punctuations)
    {
        res.processSymbol(punctuation);
        NFA expr_nfa = res.pop();
        for(int state_i : expr_nfa.end_states) // set token of acceptance states
        {
            expr_nfa.states[state_i].token = punctuation;
        }
        res.push(expr_nfa);
    }
    for(int i = 0 ; i < exprs.size() ; i++)
    {
        pair< string, vector<string> > expr = exprs[i];
        for(string token : expr.second)
        {
            if(token.compare("*") == 0)
            {
                res.kleeneStar();
            }
            else if(token.compare("+") == 0)
            {
                res.positiveClosure();
            }
            else if(token.compare("|") == 0)
            {
                res.orOp();
            }
            else if(token.compare(".") == 0)
            {
                res.concatenate();
            }
            else
            {
                res.processSymbol(token);
            }
        }
        NFA expr_nfa = res.pop();
        for(int state_i : expr_nfa.end_states) // set token of acceptance states
        {
            if(expr_nfa.states[state_i].token.compare("") == 0)
                expr_nfa.states[state_i].token = expr.first;
        }
        res.push(expr_nfa);
    }    

    res.concatenateAllStack();
    return res;
}


unordered_map<int, DFA_State> constructDFA(NFA &nfa, unordered_map<string, int> priority)
{
    unordered_map<int, set<int>> epsilon_closure_set_map = nfa.getEpsilonClosureSetMap() ;
    unordered_map<int, DFA_State> dfa_states;
    queue<set<int>> unmarked_states;
    set<int> start_state_closure;
    for (int start_state : nfa.start_states)
    {
        // insert epsilon closure of start state
        start_state_closure.insert(epsilon_closure_set_map[start_state].begin(), epsilon_closure_set_map[start_state].end());
    }

     std::cout << "nfa_end_states: ";
    for (const int& state : nfa.end_states) {
        std::cout << state << " ";
    }
    std::cout << std::endl;
    // TODO: push ep_closure(start_state) to the queue
    unmarked_states.push(start_state_closure);
    int dfa_state_id = 0;


    while(!unmarked_states.empty())
    {
        DFA_State dfa_state;
        dfa_state.nfa_states = unmarked_states.front();
        unmarked_states.pop();
        dfa_state.is_acceptance = false;
        set<int> current_states =dfa_state.nfa_states;

        // Check if the current state contains an NFA acceptance state
        for (int nfa_state : current_states)
        {
            if (find(nfa.end_states.begin(), nfa.end_states.end(), nfa_state) != nfa.end_states.end())
            {
                if(dfa_state.is_acceptance == true){
                    if(priority[dfa_state.token] > priority[nfa.states.at(nfa_state).token]){
                        dfa_state.token = nfa.states.at(nfa_state).token;
                    }
                }else{
                    dfa_state.is_acceptance = true;
                    dfa_state.token = nfa.states.at(nfa_state).token;
                }
            }
        }

        std::unordered_map<std::string, std::set<int>> combined_transitions;
        for (int state : current_states)
        {
            // Get the transitions for the current state
            const std::unordered_map<std::string, std::set<int>> &transitions = nfa.states.at(state).transitions;

            // Iterate through each transition
            for (const auto &transition : transitions)
            {
                const std::string &input_symbol = transition.first;
                const std::set<int> &target_states = transition.second;
                if(input_symbol != "\\L"){
                    combined_transitions[input_symbol].insert(target_states.begin(), target_states.end());
                }
            }
        }

        // covert each transition set to its epsilon closure
        for(auto &transition : combined_transitions)
        {
            set<int> &target_states = transition.second;

            // covert each transition set to its epsilon closure
            set<int> target_states_epsilon_closure;
            for(int target_state : target_states){
                target_states_epsilon_closure.insert(epsilon_closure_set_map[target_state].begin(), epsilon_closure_set_map[target_state].end());
            }
            combined_transitions[transition.first] = target_states_epsilon_closure;
            target_states = target_states_epsilon_closure;
        }

        dfa_state.transitions = combined_transitions;
        dfa_states[dfa_state_id] = dfa_state;
        for (const auto& entry : combined_transitions) {
            const std::set<int>& target_states = entry.second;
            bool isNotInQueue = true;
            for (std::queue<std::set<int>> tempQueue = unmarked_states; !tempQueue.empty(); tempQueue.pop()) {
                if (tempQueue.front() == target_states) {
                    isNotInQueue = false;
                    break;
                }
            }

            for (const auto& dfaEntry : dfa_states) {
            const DFA_State& existingDFAState = dfaEntry.second;
                if (existingDFAState.nfa_states == target_states) {
                    isNotInQueue = false;
                    break;
                }
            }
            if (isNotInQueue) {
                unmarked_states.push(target_states);
            }
        }
        dfa_state_id++;
    }
    return dfa_states;
}


int getDFAStateIDFromNFAStates(const std::unordered_map<int, DFA_State>& dfa_states, int target_nfa_state) {
    for (const auto& dfa_entry : dfa_states) {
        const DFA_State& dfa_state = dfa_entry.second;
        if (dfa_state.nfa_states.find(target_nfa_state) != dfa_state.nfa_states.end()) {
            return dfa_entry.first;  // Return the ID of the DFA state containing the target NFA state
        }
    }
    // If not found, return some default value indicating an error
    return -1;  // You may want to choose a more suitable default value based on your application
}
std::unordered_map<int, DFA_State> processTransitions(const std::unordered_map<int, DFA_State>& dfa_states) {
    std::unordered_map<int, DFA_State> updated_dfa_states;

    for (const auto& dfa_entry : dfa_states) {
        const DFA_State& dfa_state = dfa_entry.second;
        DFA_State updated_dfa_state = dfa_state;  // Make a copy

        for (auto& transition_entry : updated_dfa_state.transitions) {
            const std::string& transition_key = transition_entry.first;
            std::set<int>& nfa_states = transition_entry.second;

            // Create a new set to store the updated IDs
            std::set<int> updated_nfa_states;

            // Iterate over the original set and replace with corresponding IDs
            for (int nfa_state : nfa_states) {
                // Use the function to get the ID from the set of DFA states
                int dfa_state_id = getDFAStateIDFromNFAStates(dfa_states, nfa_state);

                // Add the ID to the updated set
                updated_nfa_states.insert(dfa_state_id);
            }

            // Update the transitions with the new set of IDs
            transition_entry.second = updated_nfa_states;
        }

        // Add the updated DFA state to the result map
        updated_dfa_states[dfa_entry.first] = updated_dfa_state;
    }

    return updated_dfa_states;
}


// Function to minimize a DFA
std::unordered_map<int, DFA_State> minimizeDFA(const std::unordered_map<int, DFA_State>& dfa_states) {
    std::unordered_set<int> acceptance_states;
    std::unordered_set<int> non_acceptance_states;
    for (const auto& entry : dfa_states) {
        if (entry.second.is_acceptance) {
            acceptance_states.insert(entry.first);
        } else {
            non_acceptance_states.insert(entry.first);
        }
    }
    // Helper function to check if two states are equivalent
    auto areEquivalent = [](const DFA_State& state1, const DFA_State& state2) {
    return (state1.is_acceptance == state2.is_acceptance) &&
           (state1.transitions == state2.transitions);
       };

    int id =0;
    std::vector<std::unordered_set<int>> equivalenceClassesNonAcceptance;
    std::vector<int> equivalenceClassNonIds;
    for (int state : non_acceptance_states) {
        bool found = false;
        for (auto& equivalenceClass : equivalenceClassesNonAcceptance) {
            if (areEquivalent(dfa_states.at(*equivalenceClass.begin()), dfa_states.at(state))) {
                equivalenceClass.insert(state);
                found = true;
                break;
            }
        }
        if (!found) {
            equivalenceClassesNonAcceptance.push_back({state});
            equivalenceClassNonIds.push_back(id++);
        }
    }


    std::vector<int> equivalenceClassIds;
    std::vector<std::unordered_set<int>> equivalenceClassesAcceptance;
    for (int state : acceptance_states) {
        bool found = false;
        for (auto& equivalenceClass : equivalenceClassesAcceptance) {
            if (areEquivalent(dfa_states.at(*equivalenceClass.begin()), dfa_states.at(state))) {
                equivalenceClass.insert(state);
                found = true;
                break;
            }
        }
        if (!found) {
            equivalenceClassesAcceptance.push_back({state});
            equivalenceClassIds.push_back(id++);
        }
    }

    std::unordered_map<int, DFA_State> minimizedDFA;
    for (size_t i = 0; i < equivalenceClassesAcceptance.size(); ++i) {
        int id = equivalenceClassIds[i];
        DFA_State newState;
        newState.is_acceptance = true;
        newState.token = dfa_states.at(*equivalenceClassesAcceptance[i].begin()).token; // Get token from the first state
         std::set<int> nfaStatesSet(equivalenceClassesAcceptance[i].begin(), equivalenceClassesAcceptance[i].end());
         newState.nfa_states = nfaStatesSet;
        minimizedDFA[id] = newState;
    }
    for (size_t i = 0; i < equivalenceClassesNonAcceptance.size(); ++i) {
        int id = equivalenceClassNonIds[i];
        DFA_State newState;
        newState.is_acceptance = false;
        std::set<int> nfaStatesSet(equivalenceClassesNonAcceptance[i].begin(), equivalenceClassesNonAcceptance[i].end());
         newState.nfa_states = nfaStatesSet;
        minimizedDFA[id] = newState;
    }
    for (size_t i = 0; i < equivalenceClassesAcceptance.size(); ++i) {
        int id = equivalenceClassIds[i];
        int reusult = 0;
        for (const auto& transition : dfa_states.at(*equivalenceClassesAcceptance[i].begin()).transitions) {
            std::string input = transition.first;
            int targetStateId = -1;
            for (size_t j = 0; j < equivalenceClassesAcceptance.size(); ++j) {
                auto& currentSet = equivalenceClassesAcceptance[j];
                for (const auto& entry : dfa_states) {
                    const DFA_State& dfaState = entry.second;
                    int targetNFAState = *(transition.second.begin());
        // Check if the target NFA state is present in the set of NFA states for this DFA state
                    if (dfaState.nfa_states.find(targetNFAState) != dfaState.nfa_states.end()) {
                        reusult = entry.first;
                        break; // Add the ID of the DFA state to the result set
                        }
                    }
                    auto setIterator = currentSet.find(reusult);
                        if (setIterator != currentSet.end()) {
                            targetStateId = equivalenceClassIds[j];
                            break;
                        }
            }
            if (targetStateId == -1) {
            for (size_t j = 0; j < equivalenceClassesNonAcceptance.size(); ++j) {
            auto& currentSet = equivalenceClassesNonAcceptance[j];
            auto setIterator = currentSet.find(reusult);
            if (setIterator != currentSet.end()) {
                targetStateId = equivalenceClassNonIds[j];
                break;
                }
            }
        }
        minimizedDFA[id].transitions[input].insert(targetStateId);
        }
    }

    for (size_t i = 0; i < equivalenceClassesNonAcceptance.size(); ++i) {
        int id = equivalenceClassNonIds[i];
        int reusult = 0;
        for (const auto& transition : dfa_states.at(*equivalenceClassesNonAcceptance[i].begin()).transitions) {
            std::string input = transition.first;
            int targetStateId = -1; // Default value if not found
            // Search for the target state in equivalenceClassesNonAcceptance
           for (size_t j = 0; j < equivalenceClassesNonAcceptance.size(); ++j) {
                auto& currentSet = equivalenceClassesNonAcceptance[j];
                for (const auto& entry : dfa_states) {
                    const DFA_State& dfaState = entry.second;
        // Check if the target NFA state is present in the set of NFA states for this DFA state
                    if (dfaState.nfa_states.find(*(transition.second.begin())) != dfaState.nfa_states.end()) {
                            reusult = entry.first;
                            break; // Add the ID of the DFA state to the result set
                    }
                }
                auto setIterator = currentSet.find(reusult);
                if (setIterator != currentSet.end()) {
                targetStateId = equivalenceClassNonIds[j];
                break;
                }
            }

    if (targetStateId == -1) {
    for (size_t j = 0; j < equivalenceClassesAcceptance.size(); ++j) {
        auto& currentSet = equivalenceClassesAcceptance[j];
        auto setIterator = currentSet.find(reusult);
        if (setIterator != currentSet.end()) {
            targetStateId = equivalenceClassIds[j];
            break;
                    }
                }
            }
            minimizedDFA[id].transitions[input].insert(targetStateId);
        }
    }

    return minimizedDFA;
}

int main()
{
    Parser p;
    vector<string> rules = p.get_rules_lines("D:/E/Collage/Year_4_1/Compilers/Project/Syntax-Directed-Translator/lexical_rules.txt");

    vector<string> keywords_lines = p.get_keywords_lines(rules);
    vector<string> keywords = p.parse_keywords(keywords_lines);

    vector<string> punctuations_lines = p.get_punctuation_lines(rules);
    vector<string> punctuations = p.parse_keywords(punctuations_lines);

    vector< pair<string, string> > defs_lines = p.get_regular_def_lines(rules);
    unordered_map< string, vector<string> > defs = p.parse_defs(defs_lines);

    vector< pair<string, string> > expr_lines = p.get_regular_expr_lines(rules);
    vector< pair< string, vector<string> > > exprs = p.parse_expr(expr_lines, defs);
    exprs = p.convert_exprs_to_pos(exprs);
    // for (const auto& pair : exprs) {
    //     std::cout << "key: " << pair.first << std::endl << "\t";
    //     for(string tok : pair.second)
    //         cout <<tok << " ";
    //     cout << endl;
    // }

    unordered_map<string, int> priority = getPriority(exprs, keywords, punctuations);

    NFA exprs_nfa = convert_exprs_postfix_to_NFA(exprs, keywords, punctuations, priority);
    exprs_nfa.toJSON("D:/E/Collage/Year_4_1/Compilers/Project/Syntax-Directed-Translator/NFA.json");
    cout << "NFA created successfully with size = " << exprs_nfa.states.size() << endl;

    unordered_map<int, DFA_State> dfa_states = constructDFA(exprs_nfa, priority);
    unordered_map<int, DFA_State> modified_dfa_states = processTransitions(dfa_states);
    // Open a file for writing
    
    std::ofstream outFile("D:/E/Collage/Year_4_1/Compilers/Project/Syntax-Directed-Translator/dfa_states_output.txt");

    // Check if the file is open
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return 1;
    }

    // Iterate through the DFA states map and write to the file
    for (const auto& entry : modified_dfa_states) {
        int dfa_state_id = entry.first;
        const DFA_State& dfa_state = entry.second;
        outFile << "DFA State ID: " << dfa_state_id << "\n";
        outFile << "NFA States: ";
        for (const int& nfa_state : dfa_state.nfa_states) {
            outFile << nfa_state << " ";
        }
        outFile << "\n";
        outFile << "Is Acceptance: " << (dfa_state.is_acceptance ? "true" : "false") << "\n";
        outFile << "Token: " << dfa_state.token << "\n";
        outFile << "Transitions:\n";
        for (const auto& transition : dfa_state.transitions) {
            const std::string& input_symbol = transition.first;
            const std::set<int>& target_states = transition.second;
            outFile << "  " << input_symbol << " -> ";
            for (const int& target_state : target_states) {
                outFile << target_state << " ";
            }
            outFile << "\n";
        }

        outFile << "\n";
    }

    // Close the file
    outFile.close();


    std::cout << "DFA states written to dfa_states_output.txt" << std::endl;
     unordered_map<int, DFA_State> minimzed_dfa_states =  minimizeDFA(dfa_states);


     std::size_t mapSize2 = minimzed_dfa_states.size();
    std::cout << "Size of the unordered_map: " << mapSize2 << std::endl;
    std::ofstream outFile2("D:/E/Collage/Year_4_1/Compilers/Project/Syntax-Directed-Translator/dfa_states_output2.txt");

    // Check if the file is open
    if (!outFile2.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return 1;
    }

    // Iterate through the DFA states map and write to the file
    for (const auto& entry : minimzed_dfa_states) {
        int dfa_state_id = entry.first;
        const DFA_State& dfa_state = entry.second;

        outFile2 << "DFA State Minimzed ID: " << dfa_state_id << "\n";
        outFile2 << "DFA States: ";
        for (const int& nfa_state : dfa_state.nfa_states) {
            outFile2 << nfa_state << " ";
        }
        outFile2 << "\n";
        outFile2 << "Is Acceptance: " << (dfa_state.is_acceptance ? "true" : "false") << "\n";
        outFile2 << "Token: " << dfa_state.token << "\n";

        outFile2 << "Transitions:\n";
        for (const auto& transition : dfa_state.transitions) {
            const std::string& input_symbol = transition.first;
            const std::set<int>& target_states = transition.second;

            outFile2 << "  " << input_symbol << " -> ";
            for (const int& target_state : target_states) {
                outFile2 << target_state << " ";
            }
            outFile2 << "\n";
        }

        outFile2 << "\n";
    }

    // Close the file
    outFile2.close();


    return 0;
}
