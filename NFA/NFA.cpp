#include "NFA.h"
#include <iostream>
#include <fstream>

using namespace std;

NFA::NFA() {
    stateCounter = 0;
}

void NFA::addState(int id, bool is_acceptance, bool is_start, string token) {
    State state;
    state.id = id;
    state.is_acceptance = is_acceptance;
    if (is_acceptance)
        end_states.push_back(id);
    if (is_start)
        start_states.push_back(id);
    state.token = std::move(token);
    states[id] = state;
}

void NFA::addTransition(int from, const string &symbol, int to) {
    set<int> to_states = states[from].transitions[symbol];
    to_states.insert(to);
    states[from].transitions[symbol] = to_states;
}

NFA NFA::pop() {
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();
    return nfa;
}

void NFA::push(const NFA &nfa) {
    nfa_stack.push(nfa);
}

void NFA::concatenate() {
    NFA nfa2 = nfa_stack.top();
    nfa_stack.pop();
    NFA nfa1 = nfa_stack.top();
    nfa_stack.pop();

    for (const auto& state: nfa2.states) // copy states of nfa2 to nfa1
    {
        nfa1.states[state.first] = state.second;
    }
    for (int state_i: nfa1.end_states) {
        State state = nfa1.states[state_i];
        state.is_acceptance = false;
        nfa1.states[state_i] = state;
        for (int state_j: nfa2.start_states) {
            nfa1.addTransition(state_i, "\\L", state_j);
        }
    }
    nfa1.end_states = nfa2.end_states;
    nfa_stack.push(nfa1);
}

void NFA::concatenateAllStack() {
    if (nfa_stack.empty())
        return;
    if (nfa_stack.size() == 1) {
        NFA nfa = nfa_stack.top();
        nfa_stack.pop();
        for (const auto& state: nfa.states)   // copy states
        {
            states[state.first] = state.second;
        }
        start_states = nfa.start_states;
        end_states = nfa.end_states;
        return;
    }

    start_states.clear();
    end_states.clear();
    states.clear();
    int new_start_state = stateCounter++;
    addState(new_start_state, false, true);

    while (!nfa_stack.empty()) {
        NFA nfa = nfa_stack.top();
        nfa_stack.pop();

        for (const auto& state: nfa.states)   // copy states
        {
            states[state.first] = state.second;
        }
        for (int state_i: nfa.start_states) {
            addTransition(new_start_state, "\\L", state_i);
        }
        end_states.insert(end_states.end(), nfa.end_states.begin(), nfa.end_states.end());
    }

}

void NFA::orOp() {
    // pop 2 NFAs
    NFA nfa2 = nfa_stack.top();
    nfa_stack.pop();
    NFA nfa1 = nfa_stack.top();
    nfa_stack.pop();

    // new start and end states and new NFA
    NFA new_nfa;
    int new_start_state = stateCounter++;
    int new_end_state = stateCounter++;
    new_nfa.addState(new_start_state, false, true);
    new_nfa.addState(new_end_state, true);

    // copy states
    for (const auto& state: nfa1.states) {
        new_nfa.states[state.first] = state.second;
    }
    for (const auto& state: nfa2.states) {
        new_nfa.states[state.first] = state.second;
    }

    // add transitions from the new start
    for (int state_i: nfa1.start_states) {
        new_nfa.addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i: nfa2.start_states) {
        new_nfa.addTransition(new_start_state, "\\L", state_i);
    }

    // add transitions to the new end
    for (int state_i: nfa1.end_states) {
        State state = new_nfa.states[state_i];
        state.is_acceptance = false;
        new_nfa.states[state_i] = state;
        new_nfa.addTransition(state_i, "\\L", new_end_state);
    }
    for (int state_i: nfa2.end_states) {
        State state = new_nfa.states[state_i];
        state.is_acceptance = false;
        new_nfa.states[state_i] = state;
        new_nfa.addTransition(state_i, "\\L", new_end_state);
    }

    nfa_stack.push(new_nfa);
}

void NFA::kleeneStar() {
    // pop
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();

    // new start and end states and new NFA
    NFA new_nfa;
    int new_start_state = stateCounter++;
    int new_end_state = stateCounter++;
    new_nfa.addState(new_start_state, false, true);
    new_nfa.addState(new_end_state, true);

    for (const auto& state: nfa.states) // copy states
    {
        new_nfa.states[state.first] = state.second;
    }

    for (int state_i: nfa.start_states) // new start to old start
    {
        new_nfa.addTransition(new_start_state, "\\L", state_i);
    }
    for (int state_i: nfa.end_states) // old end to new end
    {
        State state = new_nfa.states[state_i];
        state.is_acceptance = false;
        new_nfa.states[state_i] = state;
        new_nfa.addTransition(state_i, "\\L", new_end_state);
    }
    for (int state_i: nfa.end_states) // old end to starts
    {
        for (int state_j: nfa.start_states) {
            new_nfa.addTransition(state_i, "\\L", state_j);
        }
    }
    new_nfa.addTransition(new_start_state, "\\L", new_end_state);

    nfa_stack.push(new_nfa);
}

void NFA::positiveClosure() {
    NFA nfa = nfa_stack.top();
    nfa_stack.pop();

    for (int state_i: nfa.end_states) // old end to starts
    {
        for (int state_j: nfa.start_states) {
            nfa.addTransition(state_i, "\\L", state_j);
        }
    }

    nfa_stack.push(nfa);
}

void NFA::processSymbol(string symbol) {
    NFA new_nfa;
    int new_start_state = stateCounter++;
    new_nfa.addState(new_start_state, false, true);

    for (unsigned int i = 0; i < symbol.length(); i++) {
        if (i > 0 && symbol[i - 1] == '\\') {
            continue;
        }

        char c = symbol[i];
        int new_end_state = stateCounter++;
        new_nfa.addState(new_end_state);

        if (c == '\\') {
            new_nfa.addTransition(new_start_state, symbol.substr(i, 2), new_end_state);

        } else {
            new_nfa.addTransition(new_start_state, string(1, c), new_end_state);
        }
        new_start_state = new_end_state;
    }
    new_nfa.states[new_start_state].is_acceptance = true;
    new_nfa.end_states.push_back(new_start_state);

    nfa_stack.push(new_nfa);
}

unordered_map<int, set<int> > NFA::getEpsilonClosureSetMap() {
    unordered_map<int, set<int>> epsilon_closure_set;
    for (const auto& state: states) {
        int state_id = state.first;
        set<int> epsilon_closure_i;
        queue<int> q;
        q.push(state_id);
        while (!q.empty()) {
            int current_state = q.front();
            q.pop();
            epsilon_closure_i.insert(current_state);
            for (int next_state: states[current_state].transitions["\\L"]) {
                if (epsilon_closure_i.find(next_state) == epsilon_closure_i.end()) {
                    if (epsilon_closure_set.find(next_state) != epsilon_closure_set.end()) {
                        epsilon_closure_i.insert(epsilon_closure_set[next_state].begin(),
                                                 epsilon_closure_set[next_state].end());
                    } else {
                        q.push(next_state);
                    }
                }
            }
        }
        epsilon_closure_set[state_id] = epsilon_closure_i;
    }
    return epsilon_closure_set;
}

void NFA::toJSON(const string& file_path) {
    ofstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file." << endl;

        return;
    }
    file << "{" << endl;
    file << "\t\"start_states\": [" << endl;
    for (int i = 0; i < start_states.size(); i++) {
        int start_state = start_states[i];
        file << "\t\t" << start_state;
        if (i != start_states.size() - 1)
            file << ",";
        file << endl;
    }
    file << "\t]," << endl;
    file << "\t\"end_states\": [" << endl;
    for (int i = 0; i < end_states.size(); i++) {
        int start_state = end_states[i];
        file << "\t\t" << start_state;
        if (i != end_states.size() - 1)
            file << ",";
        file << endl;
    }
    file << "\t]," << endl;
    file << "\t\"states\": [" << endl;
    for (int i = 0; i < states.size(); i++) {
        State state = states[i];
        file << "\t\t{" << endl;
        file << "\t\t\t\"id\": " << i << "," << endl;
        file << "\t\t\t\"is_acceptance\": " << state.is_acceptance << "," << endl;
        file << "\t\t\t\"token\": \"" << state.token << "\"," << endl;
        file << "\t\t\t\"transitions\": [" << endl;
        int j = 0;
        for (const auto& transition: state.transitions) {
            file << "\t\t\t\t{" << endl;
            file << "\t\t\t\t\t\"symbol\": \"" << transition.first << "\"," << endl;
            file << "\t\t\t\t\t\"to\": [" << endl;
            unsigned k = 0;
            for (auto to_state: transition.second) {
                file << "\t\t\t\t\t\t" << to_state;
                if (k != transition.second.size() - 1)
                    file << ",";
                file << endl;
                k++;
            }
            file << "\t\t\t\t\t]" << endl;
            file << "\t\t\t\t}";
            if (j != state.transitions.size() - 1)
                file << ",";
            file << endl;
            j++;
        }
        file << "\t\t\t]" << endl;
        file << "\t\t}";
        if (i != states.size() - 1)
            file << ",";
        file << endl;
    }
    file << "\t]" << endl;
    file << "}" << endl;
    file.close();
}