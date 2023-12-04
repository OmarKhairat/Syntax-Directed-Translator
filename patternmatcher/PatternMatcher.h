#include <unordered_map>
#include <set>
#include <vector>
#include "parser.cpp"

#ifndef SYNTAX_DIRECTED_TRANSLATOR_TRANSITIONTABLE_H
#define SYNTAX_DIRECTED_TRANSLATOR_TRANSITIONTABLE_H


class PatternMatcher {
public:
    PatternMatcher(unordered_map<int, DFA_State> minimizedDfa);

    unordered_map<int, DFA_State> getDfa();
    void setDfa(unordered_map<int, DFA_State> minimizedDfa);

    unordered_map<string, string> matchExpression(string expression);

private:
    unordered_map<int, DFA_State> dfa;

    static int getNextTransition(const unordered_map<string, set<int>>& transitions, const string& s);
};


#endif //SYNTAX_DIRECTED_TRANSLATOR_TRANSITIONTABLE_H
