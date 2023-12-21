// First_Follow.h

#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <unordered_map>
#include <set>
#include <vector>
#include <string>

class First_Follow {
public:
    std::unordered_map<std::string, std::vector<std::string>> constructFirst(std::unordered_map<std::string, std::set<std::vector<std::string>>> inputMap);
    std::unordered_map<std::string, std::vector<std::string>> constructFollow(std::unordered_map<std::string, std::set<std::vector<std::string>>> inputMap, std::unordered_map<std::string, std::vector<std::string>> First);

private:
    bool isNonTerminal(const std::string &symbol);
    std::vector<std::pair<std::string, std::string>> findAllOccurrences(const std::unordered_map<std::string, std::set<std::vector<std::string>>> &inputMap, const std::string &key);
};

#endif // FIRST_FOLLOW_H
