#include "PatternMatcher.h"

/**
 * All commented code is left over for further assumptions.
 * */

PatternMatcher::PatternMatcher(unordered_map<int, DFA_State> minimizedDfa) {
    PatternMatcher::dfa = minimizedDfa;
//    set<string> constructedTokens;
//
//    for (const auto& pair : dfa) {
//        DFA_State state = pair.second;
//        if (state.is_acceptance) {
//            constructedTokens.insert(state.token);
//        }
//    }
//
//    PatternMatcher::tokens = constructedTokens;
}

unordered_map<int, DFA_State> PatternMatcher::getDfa() {
    return PatternMatcher::dfa;
}

void PatternMatcher::setDfa(unordered_map<int, DFA_State> minimizedDfa) {
    PatternMatcher::dfa = minimizedDfa;
}

//set<string> PatternMatcher::getTokens() {
//    return PatternMatcher::tokens;
//}

//void PatternMatcher::setTokens(set<string> newTokens) {
//    PatternMatcher::tokens = newTokens;
//}

/**
 * @brief Brief description of the function.
 *
 * When matching expressions, for each character c, check the current
 * states that can use c for pattern matching, i.e. use it for state
 * transition. If a transition occurs, replace the parent state with
 * its child, remove it otherwise.
 *
 * Encountering an acceptance state results in adding it into the
 * accepting states vector so it can be accessed later in error recovery.
 *
 * If no state can take c as a transition and the accepting states
 * vector is empty, the currently matched pattern is reported as error.
 * The pattern matcher then reports the error and continues with the
 * rest of the expression.
 *
 * @param expression The expression to be matched.
 * @return A vector of strings representing the tokens and errors (if any is found).
 *
 * Example usage:
 * @code
 * vector<string> matchedPatterns = PatternMatcher::matchExpression('xyz');
 * @endcode
 */
unordered_map<string, string> PatternMatcher::matchExpression(string expression) {
    unordered_map<string, string> symbolTable;
    string pattern;

    DFA_State currentState = dfa[0]; // Starting state
    DFA_State acceptor{};

    int next;
    int counter = 0; // Keeps track of how many characters after last accepted pattern
    bool acceptorIsPresent = false;

    unordered_map<string, set<int>> transitions; // Maintains current state transitions

    for (int i = 0; i < expression.size(); i++) {
        string s = string(1, expression[i]);
        pattern += s;
        counter++;

        if (s == "\\") {
            // Check next char
            if (i + 1 < expression.size()) {
                s += expression[i + 1];
            }
            i++;
        }

        if (currentState.is_acceptance) {
            // If there isn't any acceptor already, simply assign the current state to be the acceptor.
            if (!acceptorIsPresent) {
                acceptor = currentState;
                counter = 0;
                acceptorIsPresent = true;
            }

            // However, If the program encountered an acceptance state and
            // its token's priority is higher than current acceptor's priority,
            // Set the last acceptor to current state.
            if (currentState.priority > acceptor.priority) {
                acceptor = currentState;
                counter = 0;
            }
        }

        transitions = currentState.transitions;

        // Check if current character is valid.
        next = getNextTransition(transitions, c);

        if (next != -1) {
            currentState = dfa[next];
        } else {
            // An error has occurred. Remove the last acceptor's token from the pattern.
            if (acceptorIsPresent) {
                // If an acceptor is present, add the matched pattern and start all over.
                int idx = acceptor.token.size();
                symbolTable.insert(make_pair(acceptor.token, pattern.substr(0, idx)));

                // Decrement the counter i to start new pattern matching process.
                i -= counter;
                counter = 0;
            } else {
                // If no acceptor is available, then the pattern is reported to give an error.
                symbolTable.insert(make_pair("error", pattern));
            }
            pattern = "";
        }
    }

    return symbolTable;
}

int PatternMatcher::getNextTransition(unordered_map<std::string, set<int>> transitions, string s) {
    for (const auto& transition : transitions) {
        if (transition.first == s) {
            return *transition.second.begin();
        }
    }
    return -1;
}