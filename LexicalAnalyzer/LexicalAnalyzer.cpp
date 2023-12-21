#include "LexicalAnalyzer.h"
using namespace std;

// Implementation of LexicalAnalyzer
LexicalAnalyzer::LexicalAnalyzer(PatternMatcher thePatternMatcher)
    : patternMatcher(thePatternMatcher), counter(0) {
}

void LexicalAnalyzer::setExpression(string expression)
{
    symbolTable = patternMatcher.matchExpression(expression);
    counter = 0;
}

bool LexicalAnalyzer::hasNextToken()
{
    return counter < symbolTable.size();
}

pair<string, string> LexicalAnalyzer::getNextToken()
{
    if (hasNextToken())
    {
        return symbolTable.at(counter++);
    }
    else
    {
        return make_pair("", "");
    }
}

// helping functions
bool writeDFA(unordered_map<int, DFA::State> dfa_states, string outPath)
{
    unordered_map<int, DFA::State> modified_dfa_states = DFA::processTransitions(dfa_states);

    // Open a file for writing
    ofstream outFile(outPath + "dfa_states_output.txt");

    // Check if the file is open
    if (!outFile.is_open())
    {
        cerr << "Error opening file for writing." << endl;
        return false;
    }

    // Iterate through the DFA states map and write to the file
    for (const auto &entry : modified_dfa_states)
    {
        int dfa_state_id = entry.first;
        const DFA::State &dfa_state = entry.second;
        outFile << "DFA State ID: " << dfa_state_id << "\n";
        outFile << "NFA States: ";

        for (const int &nfa_state : dfa_state.nfa_states)
        {
            outFile << nfa_state << " ";
        }

        outFile << "\n";
        outFile << "Is Acceptance: " << (dfa_state.is_acceptance ? "true" : "false") << "\n";
        outFile << "Token: " << dfa_state.token << "\n";
        outFile << "Transitions:\n";
        for (const auto &transition : dfa_state.transitions)
        {
            const string &input_symbol = transition.first;
            const set<int> &target_states = transition.second;
            outFile << "  " << input_symbol << " -> ";
            for (const int &target_state : target_states)
            {
                outFile << target_state << " ";
            }
            outFile << "\n";
        }

        outFile << "\n";
    }

    // Close the file
    outFile.close();

    cout << "DFA states written to dfa_states_output.txt" << endl;
    return true;
}

bool writeMinimizedDFA(unordered_map<int, DFA::State> minimzed_dfa_states, string outPath)
{
    size_t mapSize2 = minimzed_dfa_states.size();
    cout << "Size of the unordered_map: " << mapSize2 << endl;
    ofstream outFile2(outPath + "dfa_states_output2.txt");

    // Check if the file is open
    if (!outFile2.is_open())
    {
        cerr << "Error opening file for writing." << endl;
        return false;
    }

    // Iterate through the DFA states map and write to the file
    for (int i = 0; i < minimzed_dfa_states.size(); i++)
    {
        int dfa_state_id = i;
        if (i == minimzed_dfa_states.size() - 1)
        {
            dfa_state_id = -2;
        }
        const DFA::State &dfa_state = minimzed_dfa_states[dfa_state_id];

        outFile2 << "DFA State Minimzed ID: " << dfa_state_id << "\n";
        outFile2 << "DFA States: ";
        for (const int &nfa_state : dfa_state.nfa_states)
        {
            outFile2 << nfa_state << " ";
        }
        outFile2 << "\n";
        outFile2 << "Is Acceptance: " << (dfa_state.is_acceptance ? "true" : "false") << "\n";
        outFile2 << "Token: " << dfa_state.token << "\n";

        outFile2 << "Transitions:\n";
        for (const auto &transition : dfa_state.transitions)
        {
            const string &input_symbol = transition.first;
            const set<int> &target_states = transition.second;

            outFile2 << "  " << input_symbol << " -> ";
            for (const int &target_state : target_states)
            {
                outFile2 << target_state << " ";
            }
            outFile2 << "\n";
        }

        outFile2 << "\n";
    }

    // Close the file
    outFile2.close();
    cout << "minimized DFA states written to dfa_states_output2.txt" << endl;

    return true;
}

// Implementation of LexicalAnalyzerFactory
LexicalAnalyzerFactory::LexicalAnalyzerFactory(string projectPath)
{
    string outPath = projectPath + R"(Outputs\)";

    // Read the lexical rules from the file and Parse them
    vector<string> rules = Parser::get_rules_lines(projectPath + "lexical_rules.txt");
    vector<string> keywords_lines = Parser::get_keywords_lines(rules);
    vector<string> keywords = Parser::parse_keywords(keywords_lines);
    vector<string> punctuations_lines = Parser::get_punctuation_lines(rules);
    vector<string> punctuations = Parser::parse_keywords(punctuations_lines);
    vector<pair<string, string>> defs_lines = Parser::get_regular_def_lines(rules);
    unordered_map<string, vector<string>> defs = Parser::parse_defs(defs_lines);
    vector<pair<string, string>> expr_lines = Parser::get_regular_expr_lines(rules);
    vector<pair<string, vector<string>>> exprs = Parser::parse_expr(expr_lines, defs);
    exprs = Parser::convert_exprs_to_pos(exprs);

    // Get the priority of each expression
    unordered_map<string, int> priority = getPriority(exprs, keywords, punctuations);

    // Convert the expressions to NFA
    NFA exprs_nfa = convert_exprs_postfix_to_NFA(exprs, keywords, punctuations);
    exprs_nfa.toJSON(outPath + "NFA.json");
    cout << "NFA created successfully with size = " << exprs_nfa.states.size() << endl;

    // Convert the NFA to DFA
    unordered_map<int, DFA::State> dfa_states = DFA::constructDFA(exprs_nfa, priority);
    writeDFA(dfa_states, outPath);

    // Minimize the DFA
    minimzed_dfa_states = DFA::minimizeDFA(dfa_states, priority);
    writeMinimizedDFA(minimzed_dfa_states, outPath);

    // get start state
    computeStartState();
}

LexicalAnalyzer LexicalAnalyzerFactory::getLexicalAnalyzer()
{
    PatternMatcher patternMatcher(minimzed_dfa_states, startState);
    LexicalAnalyzer lexicalAnalyzer(patternMatcher);
    return lexicalAnalyzer;
}

void LexicalAnalyzerFactory::computeStartState()
{
    for (const auto &entry : minimzed_dfa_states)
    {
        int dfa_state_id = entry.first;
        const DFA::State &dfa_state = entry.second;
        if (dfa_state.nfa_states.find(0) != dfa_state.nfa_states.end())
        {
            startState = dfa_state_id;
            break;
        }
    }
}

unordered_map<string, int> LexicalAnalyzerFactory::getPriority(
    vector<pair<string, vector<string>>> exprs, const vector<string> &keywords, const vector<string> &punctuations)
{
    unordered_map<string, int> priority;
    for (const string &keyword : keywords)
    {
        priority[keyword] = 0;
    }
    for (const string &punctuation : punctuations)
    {
        priority[punctuation] = 0;
    }
    for (size_t i = 0; i < exprs.size(); i++)
    {
        priority[exprs[i].first] = i + 1;
    }
    return priority;
}

NFA LexicalAnalyzerFactory::convert_exprs_postfix_to_NFA(
    const vector<pair<string, vector<string>>> &exprs, const vector<string> &keywords, const vector<string> &punctuations)
{
    NFA res;
    for (const string &keyword : keywords)
    {
        res.processSymbol(keyword);
        NFA expr_nfa = res.pop();
        for (int state_i : expr_nfa.end_states) // set token of acceptance states
        {
            expr_nfa.states[state_i].token = keyword;
        }
        res.push(expr_nfa);
    }
    for (const string &punctuation : punctuations)
    {
        res.processSymbol(punctuation);
        NFA expr_nfa = res.pop();
        for (int state_i : expr_nfa.end_states) // set token of acceptance states
        {
            expr_nfa.states[state_i].token = punctuation;
        }
        res.push(expr_nfa);
    }
    for (const auto &expr : exprs)
    {
        for (const string &token : expr.second)
        {
            if (token == "*")
            {
                res.kleeneStar();
            }
            else if (token == "+")
            {
                res.positiveClosure();
            }
            else if (token == "|")
            {
                res.orOp();
            }
            else if (token == ".")
            {
                res.concatenate();
            }
            else
            {
                res.processSymbol(token);
            }
        }
        NFA expr_nfa = res.pop();
        // set token of acceptance states
        for (int state_i : expr_nfa.end_states)
        {
            if (expr_nfa.states[state_i].token.empty())
                expr_nfa.states[state_i].token = expr.first;
        }
        res.push(expr_nfa);
    }

    res.concatenateAllStack();
    return res;
}
