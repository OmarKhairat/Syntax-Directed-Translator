#include <set>
#include <string>
#include <unordered_map>
#include "../NFA/NFA.h"

#ifndef SYNTAX_DIRECTED_TRANSLATOR_DFA_H
#define SYNTAX_DIRECTED_TRANSLATOR_DFA_H

using namespace std;

class DFA {
public:
    struct State {
        set<int> nfa_states;
        bool is_acceptance;
        string token;
        unordered_map<string, set<int>> transitions;
    };

    static int getDFAStateIDFromNFAStates(const std::unordered_map<int, State> &dfa_states, int target_nfa_state);

    static unordered_map<int, State> constructDFA(NFA &nfa, unordered_map<string, int> priority);

    static unordered_map<int, State> processTransitions(const unordered_map<int, State> &dfa_states);

    static unordered_map<int, State>
    minimizeDFA(const unordered_map<int, State> &dfa_states, unordered_map<string, int> priority);
};

#endif //SYNTAX_DIRECTED_TRANSLATOR_DFA_H
