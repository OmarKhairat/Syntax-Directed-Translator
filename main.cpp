#include <bits/stdc++.h>
#include "LexicalAnalyzer/lexicalAnalyzer.h"
#include "patternmatcher/PatternMatcher.h"
#include "Parser/parser.h"

using namespace std;

int main()
{
    Parser p;
    vector<string> rules = p.get_rules_lines("C:/Users/abdel/Desktop/Connect-4/Syntax-Directed-Translator/lexical_rules.txt");

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
    exprs_nfa.toJSON("C:/Users/abdel/Desktop/Connect-4/Syntax-Directed-Translator/NFA.json");
    cout << "NFA created successfully with size = " << exprs_nfa.states.size() << endl;

    unordered_map<int, DFA_State> dfa_states = constructDFA(exprs_nfa, priority);
    unordered_map<int, DFA_State> modified_dfa_states = processTransitions(dfa_states);
    // Open a file for writing

    std::ofstream outFile("C:/Users/abdel/Desktop/Connect-4/Syntax-Directed-Translator/Parser/dfa_states_output.txt");

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
    unordered_map<int, DFA_State> minimzed_dfa_states =  minimizeDFA(dfa_states, priority);


    std::size_t mapSize2 = minimzed_dfa_states.size();
    std::cout << "Size of the unordered_map: " << mapSize2 << std::endl;
    std::ofstream outFile2("C:/Users/abdel/Desktop/Connect-4/Syntax-Directed-Translator/Parser/dfa_states_output2.txt");

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