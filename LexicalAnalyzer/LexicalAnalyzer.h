#ifndef SYNTAX_DIRECTED_TRANSLATOR_LEXICALANALYZER_H
#define SYNTAX_DIRECTED_TRANSLATOR_LEXICALANALYZER_H

#include "../patternmatcher/PatternMatcher.h"
#include "../NFA/NFA.h"
#include "../DFA/DFA.h"
#include "../Parser/parser.h"
#include <sstream>

using namespace std;

class LexicalAnalyzer
{
public:
    explicit LexicalAnalyzer(PatternMatcher thePatternMatcher);
    void setExpression(string expression);
    pair<string, string> getNextToken();
    bool hasNextToken();

private:
    PatternMatcher patternMatcher;
    vector<pair<string, string>> symbolTable;
    size_t counter;
};

class LexicalAnalyzerFactory
{
public:
    explicit LexicalAnalyzerFactory(string projectPath);
    LexicalAnalyzer getLexicalAnalyzer();

private:
    unordered_map<int, DFA::State> minimzed_dfa_states;
    int startState;
    void computeStartState();
    unordered_map<string, int> getPriority(vector<pair<string, vector<string>>> exprs,
                                           const vector<string> &keywords,
                                           const vector<string> &punctuations);
    NFA convert_exprs_postfix_to_NFA(const vector<pair<string, vector<string>>> &exprs,
                                     const vector<string> &keywords,
                                     const vector<string> &punctuations);
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_LEXICALANALYZER_H
