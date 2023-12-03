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

//    set<string> getTokens();
//    void setTokens(set<string> newTokens);

    vector<string> matchExpression(string expression);

private:
    unordered_map<int, DFA_State> dfa;
//    set<string> tokens;

    int getNextTransition(unordered_map<string, set<int>> transitions, char c);
};


#endif //SYNTAX_DIRECTED_TRANSLATOR_TRANSITIONTABLE_H
