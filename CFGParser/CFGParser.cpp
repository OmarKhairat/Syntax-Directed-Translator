#include "CFGParser.h"

using namespace std;

string remove_all_spaces(const string &str)
{
    string res = "";
    for (char c : str)
        if (c != ' ')
            res += c;

    return res;
}

unordered_map<string, string> CFGParser::get_rules_lines(const string &rules_file_path)
{
    ifstream file(rules_file_path);
    unordered_map<string, string> rules;
    if (!file.is_open())
    {
        cerr << "Error: Unable to open the file." << endl;
        return rules;
    }

    string LHS = "", RHS = "", ruleLine = "";
    while (getline(file, ruleLine))
    {
        if (ruleLine.at(0) == '#')
        { // new rule
            if (!LHS.empty() && !RHS.empty())
                rules[remove_all_spaces(LHS)] = RHS;
            size_t pos = ruleLine.find("=");
            if (pos != string::npos)
            { // found
                LHS = ruleLine.substr(1, pos - 1);
                RHS = ruleLine.substr(pos + 1);
            }
            else
            { // not found all that in the LHS
                LHS = ruleLine.substr(1);
                RHS = "";
            }
        }
        else
        { // not start of rule
            size_t pos = ruleLine.find("=");
            if (pos != string::npos)
            { // found that the end of LHS
                LHS.append(ruleLine.substr(0, pos));
                RHS = ruleLine.substr(pos + 1);
            }
            else
            { // not found all that in the RHS
                RHS.append(ruleLine);
            }
        }
    }
    if (!LHS.empty() && !RHS.empty())
        rules[remove_all_spaces(LHS)] = RHS;
    file.close();
    return rules;
}

bool is_special_char(char c)
{
    char special_char[] = {' ', '\n', '\t', '\'', '|', '\\', '+', '-', '(', ')', '*'};
    int n = sizeof(special_char) / sizeof(special_char[0]);
    bool res = false;
    for (int i = 0; i < n && !res; i++)
    {
        res = (c == special_char[i]);
    }
    return res;
}

unordered_map<string, vector<string>> CFGParser::parse_rule(const unordered_map<string, string> &rule_lines)
{
    unordered_map<string, vector<string>> rules;
    for (auto &rule : rule_lines)
    {
        string LHS = rule.first;
        string RHS = rule.second;
        vector<string> RHS_vector;
        // split by  " ", "|", "+", "*", "(", ")", "'" unless it between ''
        size_t startToken = 0, endToken = 0;
        while (endToken < RHS.length())
        {
            if (is_special_char(RHS.at(endToken)))
            {
                if (RHS.at(endToken) == ' ' || RHS.at(endToken) == '\n' || RHS.at(endToken) == '\t')
                {
                    if (startToken != endToken)
                    { // Non-terminal
                        RHS_vector.push_back(RHS.substr(startToken, endToken - startToken));
                    } // else is just space
                }
                else if (RHS.at(endToken) == '\'')
                { // Terminal
                    if (startToken != endToken)
                    {
                        // add the prev token if existed
                        RHS_vector.push_back(RHS.substr(startToken, endToken - startToken));
                    }
                    // find the next '
                    startToken = endToken + 1;
                    endToken = RHS.find("'", startToken);
                    if (endToken != string::npos)
                    { // found
                        RHS_vector.push_back(RHS.substr(startToken, endToken - startToken));
                    }
                    else
                    {
                        // error in rules
                        return unordered_map<string, vector<string>>();
                    }
                }
                else if (RHS.at(endToken) == '\\' && endToken + 1 < RHS.length() && RHS.at(endToken + 1) == 'L')
                {
                    // epsilon
                    RHS_vector.push_back("\\L");
                    endToken++;
                }
                else
                {
                    // +, *, (, ), |
                    if (startToken != endToken)
                    {
                        // add the prev token if existed
                        RHS_vector.push_back(RHS.substr(startToken, endToken - startToken));
                    }
                    RHS_vector.push_back(RHS.substr(endToken, 1));
                }
                endToken++;
                startToken = endToken;
            }
            else
            {
                endToken++;
            }
        }
        if (startToken != endToken)
        {
            // add the prev token if existed
            RHS_vector.push_back(RHS.substr(startToken, endToken - startToken));
        }
        rules[LHS] = RHS_vector;
    }
    return rules;
}

unordered_map<string, set<vector<string>>> CFGParser::get_maped_rules(const unordered_map<string, vector<string>> &rules)
{
    unordered_map<string, set<vector<string>>> maped_rules;
    for (auto &rule : rules)
    {
        string LHS = rule.first;
        vector<string> RHS = rule.second;
        set<vector<string>> RHS_set;
        size_t start = 0;
        for (size_t end = 0; end < RHS.size(); end++)
        {
            if (RHS.at(end).compare("|") == 0)
            {
                if (start != end)
                {
                    RHS_set.insert(vector<string>(RHS.begin() + start, RHS.begin() + end));
                }
                start = end + 1;
            }
        }
        if (start != RHS.size())
        {
            RHS_set.insert(vector<string>(RHS.begin() + start, RHS.end()));
        }
        maped_rules[LHS] = RHS_set;
    }
    return maped_rules;
}

void CFGParser::topologicalSortUtil(const string &nonterminal, const unordered_map<string, set<vector<string>>> &rulesMap,
                                    set<string> &visited, stack<string> &s, set<string> &nonterminals)
{
    visited.insert(nonterminal);
    for (auto &rhs : rulesMap.at(nonterminal))
    {
        for (auto &rhs_part : rhs)
        {
            if (rulesMap.find(rhs_part) != rulesMap.end() && rhs_part != nonterminal)
            {
                if (visited.find(rhs_part) == visited.end() && nonterminals.find(rhs_part) != nonterminals.end())
                {
                    topologicalSortUtil(rhs_part, rulesMap, visited, s, nonterminals);
                }
            }
        }
    }
    s.push(nonterminal);
}

vector<pair<string, set<vector<string>>>> CFGParser::topologicalSort(const unordered_map<string, set<vector<string>>> &rulesMap)
{
    vector<pair<string, set<vector<string>>>> sorted_rules;
    unordered_map<string, int> in_degree;
    set<string> nonterminals;

    for (auto &rule : rulesMap)
    {
        in_degree[rule.first] = 0;
        nonterminals.insert(rule.first);
    }
    for (auto &rule : rulesMap)
    {
        set<vector<string>> RHS = rule.second;
        for (auto &rhs : RHS)
        {
            for (auto &rhs_part : rhs)
            {
                if (rulesMap.find(rhs_part) != rulesMap.end() && rhs_part != rule.first)
                {
                    in_degree[rhs_part]++;
                }
            }
        }
    }
    set<string> visited;
    stack<string> s;
    queue<string> q;
    for (auto &rule : rulesMap)
    {
        if (in_degree[rule.first] == 0)
        {
            q.push(rule.first);
        }
    }

    while (!q.empty())
    {
        string nonterminal = q.front();
        q.pop();
        if (visited.find(nonterminal) == visited.end())
        {
            topologicalSortUtil(nonterminal, rulesMap, visited, s, nonterminals);
        }
    }

    while (!s.empty())
    {
        const string &nonterminal = s.top();
        s.pop();
        sorted_rules.push_back({nonterminal, rulesMap.at(nonterminal)});
    }

    return sorted_rules;
}

unordered_map<string, set<vector<string>>> CFGParser::eleminateLeftRecursion(const unordered_map<string, set<vector<string>>> &rulesMap)
{
    unordered_map<string, set<vector<string>>> resultRules;
    queue<pair<string, set<vector<string>>>> rulesQueue;
    for (auto &rule : rulesMap)
    {
        rulesQueue.push(rule);
    }

    while (!rulesQueue.empty())
    {
        pair<string, set<vector<string>>> rule = rulesQueue.front();
        rulesQueue.pop();
        string nonterminal = rule.first;
        const set<vector<string>> &originalProductions = rule.second;
        set<vector<string>> newProductions;
        set<vector<string>> recursiveProductions;

        // Separate productions into recursive and non-recursive
        for (const auto &production : originalProductions)
        {
            if (production.front().compare(nonterminal) == 0)
            {
                recursiveProductions.insert(production);
            }
            else
            {
                newProductions.insert(production);
            }
        }

        if (recursiveProductions.empty())
        {
            // If no left recursion, keep the rule as it is
            resultRules[nonterminal] = originalProductions;
        }
        else
        {
            // Left recursion found, eliminate it
            string newNonterminal = nonterminal + "`";
            set<vector<string>> nonterminalProductions;

            // Create new productions for the nonterminal
            for (const auto &production : newProductions)
            {
                vector<string> newProduction = production;
                newProduction.push_back(newNonterminal);
                nonterminalProductions.insert(newProduction);
            }

            set<vector<string>> newNonterminalProductions;
            // Create new productions for the recursive nonterminal
            for (const auto &production : recursiveProductions)
            {
                vector<string> newProduction = production;
                newProduction.erase(newProduction.begin()); // Remove the left-recursive nonterminal
                newProduction.push_back(newNonterminal);
                newNonterminalProductions.insert(newProduction);
            }
            newNonterminalProductions.insert({"\\L"}); // Add epsilon production

            rulesQueue.push(make_pair(nonterminal, nonterminalProductions));
            rulesQueue.push(make_pair(newNonterminal, newNonterminalProductions));
        }
    }

    return resultRules;
}

unordered_map<string, set<vector<string>>> CFGParser::LeftFactoring(const unordered_map<string, set<vector<string>>> &rulesMap)
{
    unordered_map<string, set<vector<string>>> resultRules;

    queue<pair<string, set<vector<string>>>> rulesQueue;
    for (auto &rule : rulesMap)
    {
        rulesQueue.push(rule);
    }

    while (!rulesQueue.empty())
    {
        pair<string, set<vector<string>>> rule = rulesQueue.front();
        rulesQueue.pop();
        string nonterminal = rule.first;
        const set<vector<string>> &originalProductions = rule.second;
        set<vector<string>> newProductions;

        // Map to store common prefixes and their corresponding suffices
        unordered_map<string, vector<vector<string>>> commonPrefixes;

        // Group productions by their first symbol
        for (const auto &production : originalProductions)
        {
            if (!production.empty())
            {
                commonPrefixes[production.at(0)].push_back(production);
            }
        }

        // Process each group of productions
        for (const auto &prefixGroup : commonPrefixes)
        {
            const vector<vector<string>> &groupProductions = prefixGroup.second;
            int count = 0;

            // If there is a common prefix, factor it out
            if (groupProductions.size() > 1)
            {
                count++;
                string newNonterminal = nonterminal + to_string(count);

                // Create new productions for the nonterminal
                vector<string> newProduction;
                newProduction.push_back(prefixGroup.first);
                newProduction.push_back(newNonterminal);
                newProductions.insert(newProduction);

                // Create new productions for the new nonterminal
                set<vector<string>> newNonterminalProductions;
                for (const auto &production : groupProductions)
                {
                    vector<string> newProduction = production;
                    newProduction.erase(newProduction.begin()); // Remove the common prefix
                    if (newProduction.empty())
                    {
                        newProduction.push_back("\\L");
                    }
                    newNonterminalProductions.insert(newProduction);
                }
                rulesQueue.push(make_pair(newNonterminal, newNonterminalProductions));
            }
            else
            {
                // No common prefix, add the productions as they are
                newProductions.insert(groupProductions.begin(), groupProductions.end());
            }
        }
        resultRules[nonterminal] = newProductions;
    }

    return resultRules;
}

vector<pair<string, set<vector<string>>>> CFGParser::get_CFG_rules(const string &rules_file_path)
{
    return topologicalSort(LeftFactoring(eleminateLeftRecursion(get_maped_rules(parse_rule(get_rules_lines(rules_file_path))))));
}
