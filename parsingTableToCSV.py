import csv

# Read parsing information from the text file
parsing_info = {}
current_key = None

"""
Key: METHOD_BODY
  Token: while
    Productions: STATEMENT_LIST
  Token: float
    Productions: STATEMENT_LIST
  Token: id
    Productions: STATEMENT_LIST
  Token: int
    Productions: STATEMENT_LIST
  Token: if
    Productions: STATEMENT_LIST
"""
with open('parsing table.txt', 'r') as file:
    for line in file:
        line = line.strip()
        if line.startswith("Key: "):
            current_key = line.split("Key: ")[1]
            parsing_info[current_key] = {}
        elif line.startswith("Token: "):
            current_token = line.split("Token: ")[1]
            parsing_info[current_key][current_token] = {}
        elif line.startswith("Productions: "):
            productions = line.split("Productions: ")[1]
            parsing_info[current_key][current_token] = productions

# Extract unique tokens and keys
tokens = set(token for productions in parsing_info.values() for token in productions)
keys = list(parsing_info.keys())

# Create CSV file and write parsing table
with open('parsing_table.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)

    # Write header row
    header_row = ['Key'] + list(tokens)
    writer.writerow(header_row)

    # Write data rows
    for key in keys:
        row_data = [key] + [parsing_info[key].get(token, '') for token in tokens]
        writer.writerow(row_data)

print("Parsing table has been written to 'parsing_table.csv'")
