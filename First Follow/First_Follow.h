// First_Follow.h

#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <unordered_map>
#include <set>
#include <vector>
#include <string>

using namespace std;

class First_Follow {
public:
    std::unordered_map<std::string, std::vector<std::string>> constructFirst(std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> inputMap, const set<string> &nonTerminals);
    std::unordered_map<std::string, std::vector<std::string>> constructFollow(std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> inputMap, std::unordered_map<std::string, std::vector<std::string>> First, const set<string> &nonTerminals);
    static bool isNonTerminal(const std::string &symbol, const set<string> &nonTerminals);
    static std::vector<std::pair<std::string, std::string>> findAllOccurrences(
        const std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> &inputMap,
        const std::string &key);
};

#endif // FIRST_FOLLOW_H
