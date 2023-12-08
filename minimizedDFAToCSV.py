import csv

def read_dfa_from_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    dfas = []
    current_dfa = None
    for line in lines:
        line = line.strip()
        if line.startswith("DFA State Minimzed ID:"):
            if current_dfa:
                dfas.append(current_dfa)
            current_dfa = {"DFA State Minimzed ID": int(line.split(":")[1].strip())}
        elif line.startswith("DFA States:"):
            current_dfa["DFA States"] = list(map(int, line.split(":")[1].strip().split()))
        elif line.startswith("Is Acceptance:"):
            current_dfa["Is Acceptance"] = line.split(":")[1].strip() == "true"
        elif line.startswith("Token:"):
            current_dfa["Token"] = line.split(":")[1].strip()
        elif line.startswith("Transitions:"):
            current_dfa["Transitions"] = {}
        elif line:
            transition_parts = line.split("->")
            symbol = transition_parts[0].strip()
            target_state = int(transition_parts[1].strip())
            current_dfa["Transitions"][symbol] = target_state

    if current_dfa:
        dfas.append(current_dfa)

    # sort dfas by their ID
    dfas = sorted(dfas, key=lambda dfa: dfa["DFA State Minimzed ID"])
    return dfas

def write_dfa_to_csv(dfas, csv_file_path):
    all_transitions = set()
    for dfa in dfas:
        if "Transitions" in dfa:
            all_transitions.update(dfa["Transitions"].keys())

    all_transitions = sorted(all_transitions)

    headers = ["DFA State Minimzed ID", "Is Acceptance", "Token"] + all_transitions

    with open(csv_file_path, 'w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(headers)

        for dfa in dfas:
            row = [dfa["DFA State Minimzed ID"], dfa["Is Acceptance"], dfa["Token"]]
            if "Transitions" in dfa:
                for symbol in all_transitions:
                    target_state = dfa["Transitions"].get(symbol, "")
                    row.append(target_state)
            writer.writerow(row)

# Example usage:
input_file_path = 'Outputs/dfa_states_output2.txt'
output_csv_path = 'Outputs/minimizedDFA.csv'

dfas = read_dfa_from_file(input_file_path)
write_dfa_to_csv(dfas, output_csv_path)