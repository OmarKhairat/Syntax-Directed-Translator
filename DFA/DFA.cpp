#include "DFA.h"

using namespace std;

int DFA::getDFAStateIDFromNFAStates(const unordered_map<int, DFA::State> &dfa_states, int target_nfa_state) {
    for (const auto &dfa_entry: dfa_states) {
        const State &dfa_state = dfa_entry.second;
        if (dfa_state.nfa_states.find(target_nfa_state) != dfa_state.nfa_states.end()) {
            return dfa_entry.first; // Return the ID of the DFA state containing the target NFA state
        }
    }
    // If not found, return -1 as default value indicating an error
    return -1;
}

unordered_map<int, DFA::State> DFA::constructDFA(NFA &nfa, unordered_map<string, int> priority) {
    unordered_map<int, set<int>> epsilon_closure_set_map = nfa.getEpsilonClosureSetMap();
    unordered_map<int, State> dfa_states;
    queue<set<int>> unmarked_states;
    set<int> start_state_closure;

    for (int start_state: nfa.start_states) {
        // insert epsilon closure of start state
        start_state_closure.insert(epsilon_closure_set_map[start_state].begin(),
                                   epsilon_closure_set_map[start_state].end());
    }

    unmarked_states.push(start_state_closure);
    int dfa_state_id = 0;
    set<string> input_symbols;
    for (const auto &state: nfa.states) {
        for (const auto &transition: state.second.transitions) {
            if (transition.first != "\\L") {
                input_symbols.insert(transition.first);
            }
        }
    }
    int DEAD_STATE_ID = -2;
    State dead_state;
    dead_state.is_acceptance = false;
    dfa_states[DEAD_STATE_ID] = dead_state;
    while (!unmarked_states.empty()) {
        State dfa_state;
        dfa_state.nfa_states = unmarked_states.front();
        unmarked_states.pop();
        dfa_state.is_acceptance = false;
        set<int> current_states = dfa_state.nfa_states;

        // Check if the current state contains an NFA acceptance state
        for (int nfa_state: current_states) {
            if (find(nfa.end_states.begin(), nfa.end_states.end(), nfa_state) != nfa.end_states.end()) {
                if (dfa_state.is_acceptance) {
                    if (priority[dfa_state.token] > priority[nfa.states.at(nfa_state).token]) {
                        dfa_state.token = nfa.states.at(nfa_state).token;
                    }
                } else {
                    dfa_state.is_acceptance = true;
                    dfa_state.token = nfa.states.at(nfa_state).token;
                }
            }
        }

        std::unordered_map<std::string, std::set<int>> combined_transitions;
        for (int state: current_states) {
            // Get the transitions for the current state
            const std::unordered_map<std::string, std::set<int>> &transitions = nfa.states.at(state).transitions;

            // Iterate through each transition
            for (const auto &transition: transitions) {
                const std::string &input_symbol = transition.first;
                const std::set<int> &target_states = transition.second;
                if (input_symbol != "\\L") {
                    combined_transitions[input_symbol].insert(target_states.begin(), target_states.end());
                }
            }
        }

        for (const auto &symbol: input_symbols) {
            if (combined_transitions.find(symbol) == combined_transitions.end()) {
                combined_transitions[symbol].insert(DEAD_STATE_ID);
            }
        }
        // covert each transition set to its epsilon closure
        for (auto &transition: combined_transitions) {
            set<int> &target_states = transition.second;

            // covert each transition set to its epsilon closure
            set<int> target_states_epsilon_closure;
            for (int target_state: target_states) {
                target_states_epsilon_closure.insert(epsilon_closure_set_map[target_state].begin(),
                                                     epsilon_closure_set_map[target_state].end());
            }
            combined_transitions[transition.first] = target_states_epsilon_closure;
            target_states = target_states_epsilon_closure;
        }
        dfa_state.transitions = combined_transitions;
        dfa_states[dfa_state_id] = dfa_state;
        for (const auto &entry: combined_transitions) {
            const std::set<int> &target_states = entry.second;
            bool isNotInQueue = true;
            for (std::queue<std::set<int>> tempQueue = unmarked_states; !tempQueue.empty(); tempQueue.pop()) {
                if (tempQueue.front() == target_states) {
                    isNotInQueue = false;
                    break;
                }
            }

            for (const auto &dfaEntry: dfa_states) {
                const State &existingDFAState = dfaEntry.second;
                if (existingDFAState.nfa_states == target_states) {
                    isNotInQueue = false;
                    break;
                }
            }
            if (isNotInQueue) {
                unmarked_states.push(target_states);
            }
        }
        dfa_state_id++;
    }
    return dfa_states;
}

unordered_map<int, DFA::State> DFA::processTransitions(const unordered_map<int, DFA::State> &dfa_states) {
    std::unordered_map<int, State> updated_dfa_states;

    for (const auto &dfa_entry: dfa_states) {
        const State &dfa_state = dfa_entry.second;
        State updated_dfa_state = dfa_state; // Make a copy
        for (auto &transition_entry: updated_dfa_state.transitions) {
            std::set<int> &nfa_states = transition_entry.second;
            // Create a new set to store the updated IDs
            std::set<int> updated_nfa_states;
            for (const auto &entry: dfa_states) {
                const std::set<int> &nfaStatesSet = entry.second.nfa_states;
                if (nfaStatesSet == nfa_states) {
                    int dfa_state_id = entry.first;
                    updated_nfa_states.insert(dfa_state_id);
                    transition_entry.second = updated_nfa_states;
                    break;
                }
                /*
                    // Iterate over the original set and replace with corresponding IDs
                    for (int nfa_state : nfa_states) {
                        // Use the function to get the ID from the set of DFA states
                        int dfa_state_id = getDFAStateIDFromNFAStates(dfa_states, nfa_state);

                        // Add the ID to the updated set
                        updated_nfa_states.insert(dfa_state_id);
                    }
                    */
                // Update the transitions with the new set of IDs
            }

            // Add the updated DFA state to the result map
        }
        updated_dfa_states[dfa_entry.first] = updated_dfa_state;
    }
    return updated_dfa_states;
}

int
DFA::findIndex(const vector<std::unordered_set<int>> &equivalenceClasses, const unordered_set<int> &targetSet) {
    for (size_t i = 0; i < equivalenceClasses.size(); ++i) {
        if (equivalenceClasses[i] == targetSet) {
            return static_cast<int>(i); // Return the index if a match is found
        }
    }
    return -1; // Return -1 if no match is found
}

// Function to minimize a DFA
unordered_map<int, DFA::State>
DFA::minimizeDFA(const unordered_map<int, State> &dfa_states, unordered_map<string, int> priority) {
    std::unordered_set<int> acceptance_states;
    std::unordered_set<int> non_acceptance_states;
    std::unordered_map<int, State> minimizedDFA;
    for (const auto &entry: dfa_states) {
        if (entry.first != -2) {
            if (entry.second.is_acceptance) {
                acceptance_states.insert(entry.first);
            } else {
                non_acceptance_states.insert(entry.first);
            }
        }
    }

    int DEAD_STATE_ID = -2;
    State dead_state;
    dead_state.is_acceptance = false;
    dead_state.nfa_states = {-2};
    std::set<std::string> input_symbols;
    for (const auto &entry: dfa_states) {
        for (const auto &transition: entry.second.transitions) {
            if (transition.first != "\\L") {
                input_symbols.insert(transition.first);
            }
        }
    }

    minimizedDFA[DEAD_STATE_ID] = dead_state;

    // Helper function to check if two states are equivalent
    auto areEquivalent = [](const State &state1, const State &state2) {
        return (state1.is_acceptance == state2.is_acceptance) &&
               (state1.transitions == state2.transitions);
    };

    auto areEquivalentWithToken = [](const State &state1, const State &state2) {
        return (state1.is_acceptance == state2.is_acceptance) &&
               (state1.token == state2.token) &&
               (state1.transitions == state2.transitions);
    };
    int id = 0;
    std::vector<std::unordered_set<int>> equivalenceClassesNonAcceptance;
    std::vector<int> equivalenceClassNonIds;
    for (int state: non_acceptance_states) {
        bool found = false;
        for (auto &equivalenceClass: equivalenceClassesNonAcceptance) {
            if (areEquivalent(dfa_states.at(*equivalenceClass.begin()), dfa_states.at(state))) {
                equivalenceClass.insert(state);
                found = true;
                break;
            }
        }
        if (!found) {
            equivalenceClassesNonAcceptance.push_back({state});
            equivalenceClassNonIds.push_back(id++);
        }
    }

    std::vector<int> equivalenceClassIds;
    std::vector<std::unordered_set<int>> equivalenceClassesAcceptance;
    for (int state: acceptance_states) {
        bool found = false;
        for (auto &equivalenceClass: equivalenceClassesAcceptance) {
            if (areEquivalentWithToken(dfa_states.at(*equivalenceClass.begin()), dfa_states.at(state))) {
                equivalenceClass.insert(state);
                found = true;
                break;
            }
        }
        if (!found) {
            equivalenceClassesAcceptance.push_back({state});
            equivalenceClassIds.push_back(id++);
        }
    }

    for (size_t i = 0; i < equivalenceClassesAcceptance.size(); ++i) {
        int newId = equivalenceClassIds[i];
        State newState;
        newState.is_acceptance = true;
        // get most priority token
        unordered_set<int> &equal_states = equivalenceClassesAcceptance[i];
        string most_priority_token;
        for (auto state: equal_states) {
            if (most_priority_token.empty()) {
                most_priority_token = dfa_states.at(state).token;
            } else {
                if (priority[dfa_states.at(state).token] < priority[most_priority_token]) {
                    most_priority_token = dfa_states.at(state).token;
                }
            }
        }
        newState.token = most_priority_token;
        std::set<int> nfaStatesSet(equivalenceClassesAcceptance[i].begin(), equivalenceClassesAcceptance[i].end());
        newState.nfa_states = nfaStatesSet;
        minimizedDFA[newId] = newState;
    }
    for (size_t i = 0; i < equivalenceClassesNonAcceptance.size(); ++i) {
        int newId = equivalenceClassNonIds[i];
        State newState;
        newState.is_acceptance = false;
        std::set<int> nfaStatesSet(equivalenceClassesNonAcceptance[i].begin(),
                                   equivalenceClassesNonAcceptance[i].end());
        newState.nfa_states = nfaStatesSet;
        minimizedDFA[newId] = newState;
    }
    for (size_t i = 0; i < equivalenceClassesAcceptance.size(); ++i) {
        int newId = equivalenceClassIds[i];
        int result = 0;
        for (const auto &transition: dfa_states.at(*equivalenceClassesAcceptance[i].begin()).transitions) {
            std::string input = transition.first;
            std::set<int> mySet = transition.second;
            if (mySet.empty()) {
                minimizedDFA[newId].transitions[input].insert(DEAD_STATE_ID);
            } else {
                int targetStateId = -1;
                for (size_t j = 0; j < equivalenceClassesAcceptance.size(); ++j) {
                    auto &currentSet = equivalenceClassesAcceptance[j];
                    for (const auto &entry: dfa_states) {
                        // Check if the target NFA state is present in the set of NFA states for this DFA state
                        if (entry.second.nfa_states == mySet) {
                            result = entry.first;
                            break; // Add the ID of the DFA state to the result set
                        }
                    }
                    auto setIterator = currentSet.find(result);
                    if (setIterator != currentSet.end()) {
                        int index = findIndex(equivalenceClassesAcceptance, currentSet);
                        targetStateId = equivalenceClassIds[index];
                        break;
                    }
                }
                if (targetStateId == -1) {
                    for (size_t j = 0; j < equivalenceClassesNonAcceptance.size(); ++j) {
                        auto &currentSet = equivalenceClassesNonAcceptance[j];
                        auto setIterator = currentSet.find(result);
                        if (setIterator != currentSet.end()) {
                            int index = findIndex(equivalenceClassesNonAcceptance, currentSet);
                            targetStateId = equivalenceClassNonIds[index];
                            break;
                        }
                    }
                }
                minimizedDFA[newId].transitions[input].insert(targetStateId);
            }
        }
    }

    for (size_t i = 0; i < equivalenceClassesNonAcceptance.size(); ++i) {
        int newId = equivalenceClassNonIds[i];
        int result = 0;
        for (const auto &transition: dfa_states.at(*equivalenceClassesNonAcceptance[i].begin()).transitions) {
            std::string input = transition.first;
            std::set<int> mySet = transition.second;

            if (mySet.empty()) {
                minimizedDFA[newId].transitions[input].insert(DEAD_STATE_ID);
            } else {
                int targetStateId = -1; // Default value if not found
                // Search for the target state in equivalenceClassesNonAcceptance
                for (size_t j = 0; j < equivalenceClassesNonAcceptance.size(); ++j) {
                    auto &currentSet = equivalenceClassesNonAcceptance[j];
                    for (const auto &entry: dfa_states) {
                        // Check if the target NFA state is present in the set of NFA states for this DFA state
                        if (entry.second.nfa_states == mySet) {
                            result = entry.first;
                            break; // Add the ID of the DFA state to the result set
                        }
                    }

                    auto setIterator = currentSet.find(result);
                    if (setIterator != currentSet.end()) {
                        int index = findIndex(equivalenceClassesNonAcceptance, currentSet);
                        targetStateId = equivalenceClassNonIds[index];
                        minimizedDFA[newId].transitions[input].insert(targetStateId);
                        break;
                    }
                }

                if (targetStateId == -1) {

                    for (size_t j = 0; j < equivalenceClassesAcceptance.size(); ++j) {
                        auto &currentSet = equivalenceClassesAcceptance[j];
                        auto setIterator = currentSet.find(result);
                        if (setIterator != currentSet.end()) {
                            int index = findIndex(equivalenceClassesAcceptance, currentSet);
                            targetStateId = equivalenceClassIds[index];
                            minimizedDFA[newId].transitions[input].insert(targetStateId);
                            break;
                        }
                    }
                }
            }
        }
    }

    return minimizedDFA;
}