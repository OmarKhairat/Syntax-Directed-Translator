#ifndef SYNTAX_DIRECTED_TRANSLATOR_NFA_H
#define SYNTAX_DIRECTED_TRANSLATOR_NFA_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

class NFA
{
public:
    NFA();

    NFA pop();

    void push(const NFA &nfa);

    void concatenate();

    void concatenateAllStack();

    void orOp();

    void kleeneStar();

    void positiveClosure();

    void processSymbol(string symbol);

    void toJSON(const string &file_path);

    void addState(int id, bool is_acceptance = false, bool is_start = false, string token = "");

    void addTransition(int from, const string &symbol, int to);

    unordered_map<int, set<int>> getEpsilonClosureSetMap();

    struct State
    {
        int id{};
        bool is_acceptance{};
        string token; // if is_acceptance is true
        unordered_map<string, set<int>> transitions;
    };

    vector<int> start_states;
    vector<int> end_states;
    unordered_map<int, State> states{};

private:
    stack<NFA> nfa_stack;
    int stateCounter;
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_NFA_H
