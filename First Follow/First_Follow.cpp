#include "First_Follow.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <string>
#include <unordered_set>
bool First_Follow::isNonTerminal(const std::string &symbol) {
    // Check if the symbol is not empty
    if (symbol.empty()) {
        return false;
    }

    // Check if the symbol is all uppercase, ends with "'", or contains at least one underscore
    return (std::all_of(symbol.begin(), symbol.end(), ::isupper) ||
            (symbol.back() == '\'' &&
             std::all_of(symbol.begin(), symbol.end() - 1, ::isupper)) ||
            (std::any_of(symbol.begin(), symbol.end(), [](char c) { return c == '_'; }))) &&
           (symbol != "Epsilon");
}

std::unordered_map<std::string, std::vector<std::string>> First_Follow::constructFirst(std::unordered_map<std::string, std::set<std::vector<std::string>>> inputMap)
{
    std::unordered_map<std::string, std::vector<std::string>> result;

  for (auto it = inputMap.begin(); it != inputMap.end(); ++it)
        {
            const std::string &nonTerminal = it->first;
            const std::set<std::vector<std::string>> &productions = it->second;

            for (const std::vector<std::string> &production : productions)
            {
                bool containsEpsilon = true; // Assume all symbols have epsilon initially

                for (const std::string &symbol : production)
                {
                    if (isNonTerminal(symbol))
                    {
                        auto findResult = result.find(symbol);
                        if (findResult != result.end())
                        {
                            // Check for duplicates before inserting into the vector
                            for (const auto &elem : findResult->second)
                            {
                                if (std::find(result[nonTerminal].begin(), result[nonTerminal].end(), elem) == result[nonTerminal].end())
                                {
                                    result[nonTerminal].push_back(elem);
                                }
                            }

                            containsEpsilon &= (std::find(findResult->second.begin(), findResult->second.end(), "Epsilon") != findResult->second.end());
                            if (!containsEpsilon)
                                break;
                        }
                    }
                    else
                    {
                        // Check for duplicates before inserting into the vector
                        if (std::find(result[nonTerminal].begin(), result[nonTerminal].end(), symbol) == result[nonTerminal].end())
                        {
                            result[nonTerminal].push_back(symbol);
                        }

                        containsEpsilon = false;
                        break;
                    }
                }

                if (containsEpsilon && std::all_of(production.begin(), production.end(), [=](const std::string &symbol)
                                                { return isNonTerminal(symbol) && result.at(symbol).empty(); }))
                {
                    // Check for duplicates before inserting into the vector
                    if (std::find(result[nonTerminal].begin(), result[nonTerminal].end(), "Epsilon") == result[nonTerminal].end())
                    {
                        result[nonTerminal].push_back("Epsilon");
                    }
                }
            }
        }

    return result;
}


std::vector<std::pair<std::string, std::string>> First_Follow::findAllOccurrences(
    const std::unordered_map<std::string, std::set<std::vector<std::string>>> &inputMap,
    const std::string &key)
{

    std::vector<std::pair<std::string, std::string>> occurrences;

    for (const auto &entry : inputMap)
    {
        const std::set<std::vector<std::string>> &currentSet = entry.second;

        // Check if the key is present in the current set
        for (const auto &vectorOfString : currentSet)
        {
            auto keyIterator = std::find(vectorOfString.begin(), vectorOfString.end(), key);

            if (keyIterator != vectorOfString.end())
            {
                // Key found in the current set, retrieve the next element and the key of the set
                auto nextElementIterator = std::next(keyIterator);

                std::string nextElement = (nextElementIterator != vectorOfString.end()) ? *nextElementIterator : "";
                std::string setKey = entry.first;
                occurrences.push_back(std::make_pair(nextElement, setKey));
            }
        }
    }

    return occurrences;
}

std::unordered_map<std::string, std::vector<std::string>> First_Follow::constructFollow(std::unordered_map<std::string, std::set<std::vector<std::string>>> inputMap, std::unordered_map<std::string, std::vector<std::string>> First)
{
    std::unordered_map<std::string, std::vector<std::string>> result;
    //auto it = std::prev(inputMap.end());
   auto it = inputMap.begin();
    const std::string &firstKey = it->first;
    result[firstKey].push_back("$");
    for (const auto &entry : inputMap)
    {
        const std::string &currentKey = entry.first;
        auto occurrences = findAllOccurrences(inputMap, currentKey);
        std::unordered_set<std::string> seenElements;
        for (const auto &occurrence : occurrences)
        {
            const std::string &nextElement = occurrence.first;
            const std::string &setKey = occurrence.second;
             if (seenElements.find(nextElement) != seenElements.end()) {
            // If seen, skip this occurrence
            continue;
        }
            if (isNonTerminal(nextElement))
            {
                // If the next element is a non-terminal, append the First set of the next element to the result
                auto &firstSetOfNextElement = First[nextElement];
                result[currentKey].insert(result[currentKey].end(), firstSetOfNextElement.begin(), firstSetOfNextElement.end());
                // If the First set of the next element contains epsilon, exclude "Epsilon" and append the Follow set of the set key to the result
                auto epsilonFound = std::find(firstSetOfNextElement.begin(), firstSetOfNextElement.end(), "Epsilon");
                if (epsilonFound != firstSetOfNextElement.end())
                {
                    // Exclude "Epsilon" from the result
                    result[currentKey].erase(std::remove(result[currentKey].begin(), result[currentKey].end(), "Epsilon"), result[currentKey].end());

                    // Append the Follow set of the set key to the result
                    result[currentKey].insert(result[currentKey].end(), result[setKey].begin(), result[setKey].end());
                }
                 seenElements.insert(nextElement);
            }
            else if (nextElement.empty())
            {

                // append the Follow set of the set key to the result
                result[currentKey].insert(result[currentKey].end(), result[setKey].begin(), result[setKey].end());
            }
            else
            {
                // If the next element is a terminal, append it to the result
                result[currentKey].push_back(nextElement);
            }
        }
    }

    return result;
}

