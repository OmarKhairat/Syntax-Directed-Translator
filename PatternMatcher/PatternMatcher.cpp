#include "PatternMatcher.h"

#include <utility>

/**
 * All commented code is left over for further assumptions.
 * */

PatternMatcher::PatternMatcher(unordered_map<int, DFA::State> minimizedDfa) {
    PatternMatcher::dfa = std::move(minimizedDfa);
}

unordered_map<int, DFA::State> PatternMatcher::getDfa() {
    return PatternMatcher::dfa;
}

void PatternMatcher::setDfa(unordered_map<int, DFA::State> minimizedDfa) {
    PatternMatcher::dfa = std::move(minimizedDfa);
}

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
 * vector<pair<string, string>> matchedPatterns = PatternMatcher::matchExpression('xyz');
 * @endcode
 */
vector<pair<string, string>> PatternMatcher::matchExpression(string expression) {
    vector<pair<string, string>> symbolTable;
    string pattern;

    DFA::State currentState = dfa[0]; // Starting state
    DFA::State acceptor{};
    cout << "State 0 is accepting? " << currentState.is_acceptance << endl;

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
            counter++;
            pattern += expression[i + 1];
        }

        if (currentState.is_acceptance) {
            // If there isn't any acceptor already, simply assign the current state to be the acceptor.
            if (!acceptorIsPresent) {
                acceptor = currentState;
                counter = 0;
                acceptorIsPresent = true;
            } else {
                acceptor = currentState;
                counter = 0;
            }
        }

        // Check if current character is valid.
        transitions = currentState.transitions;
        next = getNextTransition(transitions, s);

        if (next != -1) {
            currentState = dfa[next];
            cout << "State: " << next << ", Is Accepting?: " << currentState.is_acceptance << ", Token = "
                 << currentState.token << endl << endl;
        }

        if (i == expression.size() - 1 || next == -1) {
            cout << "ENTERED TOKEN DETECTION PHASE" << endl;
            // An error has occurred. Remove the last acceptor's token from the pattern.
            if (acceptorIsPresent) {
                cout << "Acceptor is present. Token = " << acceptor.token << ", Pattern = " << pattern << endl;
                cout << counter << endl;
                // If an acceptor is present, add the matched pattern and start all over.
                size_t idx = acceptor.token.size();
                symbolTable.emplace_back(acceptor.token, pattern.substr(0, idx));

                // Decrement the counter i to start new pattern matching process.
                i -= counter;
                counter = 0;
            } else {
                cout << "Acceptor is not present." << endl;
                // If no acceptor is available, then the pattern is reported to give an error.
                symbolTable.emplace_back("error", pattern);
            }

            // Reset all variables.
            pattern = "";
            acceptorIsPresent = false;
            acceptor = {};
        }
    }

    return symbolTable;
}

int PatternMatcher::getNextTransition(const unordered_map<std::string, set<int>> &transitions, const string &s) {
    for (const auto &transition: transitions) {
        if (transition.first == s) {
            cout << transition.first << " --> " << *transition.second.begin() << endl;
            return *transition.second.begin();
        }
    }
    return -1;
}