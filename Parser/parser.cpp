#include "parser.h"
#include <unordered_map>
#include <stack>
#include <set>
#include <bits/stdc++.h>

using namespace std;

// -------------------NFA-------------------
struct NFA_State
{
    int id;
    bool is_acceptance;
    string token; // if is_acceptance is true
    unordered_map< string, vector<int> > transitions;
};

class NFA {
public:
    NFA();
    void addState(int id, bool is_acceptance = false, bool is_start = false , string token = "");
    void addTransition(int from, string symbol, int to);
    void concatenate();
    void or_op();
    void kleeneStar();
    void positiveClosure();
    void processSymbol(string symbol);
    unordered_map<int, NFA_State> states;
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
    vector<int> to_stats = states[from].transitions[symbol];
    to_stats.push_back(to);
    states[from].transitions[symbol] = to_stats;
}

void NFA::concatenate()
{
    NFA nfa2 = nfa_stack.top();
    nfa_stack.pop();
    NFA nfa1 = nfa_stack.top();
    nfa_stack.pop();

    for (int state_i : nfa1.end_states)
    {
        NFA_State state = nfa1.states[state_i];
        state.is_acceptance = false;
        for (int state_j : nfa2.start_states)
        {
            nfa1.addTransition(state_i, "\\L", state_j);
        }
    }
    for(auto state : nfa2.states)
    {
        nfa1.states[state.first] = state.second;
    }
    nfa1.end_states = nfa2.end_states;
    nfa_stack.push(nfa1);
}

void NFA::or_op()
{
    NFA nfa2 = nfa_stack.top();
    nfa_stack.pop();
    NFA nfa1 = nfa_stack.top();
    nfa_stack.pop();

    int new_start_state = stateCounter++;
    int new_end_state = stateCounter++;

    addState(new_start_state, false, true);
    addState(new_end_state, true);

    for (int state_i : nfa1.start_states)
    {
        addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i : nfa2.start_states)
    {
        addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i : nfa1.end_states)
    {
        NFA_State state = nfa1.states[state_i];
        state.is_acceptance = false;
        addTransition(state_i, "\\L", new_end_state);
    }
    for (int state_i : nfa2.end_states)
    {
        NFA_State state = nfa2.states[state_i];
        state.is_acceptance = false;
        addTransition(state_i, "\\L", new_end_state);
    }

    for(auto state : nfa1.states)
    {
        states[state.first] = state.second;
    }
    for(auto state : nfa2.states)
    {
        states[state.first] = state.second;
    }
    end_states.push_back(new_end_state);
    start_states.push_back(new_start_state);

    nfa_stack.push(*this);
}

void NFA::kleeneStar()
{
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();

    int new_start_state = stateCounter++;
    int new_end_state = stateCounter++;

    addState(new_start_state, false, true);
    addState(new_end_state, true);

    for (int state_i : nfa.start_states) // new start to old start
    {
        addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i : nfa.end_states) // old end to new end
    {
        NFA_State state = nfa.states[state_i];
        state.is_acceptance = false;
        addTransition(state_i, "\\L", new_end_state);
    }
    for (int state_i : nfa.end_states) // old end to starts
    {
        for (int state_j : nfa.start_states)
        {
            addTransition(state_i, "\\L", state_j);
        }
    }

    for(auto state : nfa.states)
    {
        states[state.first] = state.second;
    }
    end_states.push_back(new_end_state);
    start_states.push_back(new_start_state);

    nfa_stack.push(*this);

}

void NFA::positiveClosure()
{
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();

    for (int state_i : nfa.end_states) // old end to starts
    {
        for (int state_j : nfa.start_states)
        {
            addTransition(state_i, "\\L", state_j);
        }
    }

    for(auto state : nfa.states)
    {
        states[state.first] = state.second;
    }
    nfa_stack.push(*this);
}

void NFA::processSymbol(string symbol)
{
    int new_start_state = stateCounter++;
    addState(new_start_state, false, true);

    for(int i=0 ; i<symbol.length() ; i++)
    {
        if(i>0 && symbol[i--] == '\\')
        {
            continue;
        }

        char c = symbol[i];
        int new_end_state = stateCounter++;
        addState(new_end_state);

        if(c == '\\')
        {
            addTransition(new_start_state, symbol.substr(i, 2), new_end_state);

        }else{
            addTransition(new_start_state, string(1, c), new_end_state);
        }
        new_start_state = new_end_state;
    }
    states[new_start_state].is_acceptance = true;
    end_states.push_back(new_start_state);

    nfa_stack.push(*this);
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


NFA convert_expr_postfix_to_NFA(vector< pair< string, vector<string> > > exprs)
{
    NFA res;
    for(pair< string, vector<string> > expr : exprs)
    {
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
                res.or_op();
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
        for(int state_i : res.end_states)
        {
            NFA_State state = res.states[state_i];
            if(state.token.compare("") == 0){
                state.token = expr.first;
                res.states[state_i] = state;
            }
        }
    }
    return res;
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
    for (const auto& pair : exprs) {
        std::cout << "key: " << pair.first << std::endl << "\t";
        for(string tok : pair.second)
            cout <<tok << " ";
        cout << endl;
    }

    NFA exprs_nfa = convert_expr_postfix_to_NFA(exprs);
    //print exprs_nfa
    for(auto state : exprs_nfa.states)
    {
        cout << "state: " << state.first << endl;
        cout << "\tis_acceptance: " << state.second.is_acceptance << endl;
        cout << "\ttoken: " << state.second.token << endl;
        cout << "\ttransitions: " << endl;
        for(auto transition : state.second.transitions)
        {
            cout << "\t\t" << transition.first << ": ";
            for(int to_state : transition.second)
            {
                cout << to_state << " ";
            }
            cout << endl;
        }
    }

    
    return 0;
}