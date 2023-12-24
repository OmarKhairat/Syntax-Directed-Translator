#include "First_Follow.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <string>
#include <unordered_set>
bool First_Follow::isNonTerminal(const std::string &symbol, const set<string> &nonTerminals)
{
    return nonTerminals.find(symbol) != nonTerminals.end();
}

std::unordered_map<std::string, std::vector<std::string>> First_Follow::constructFirst(std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> inputMap, const set<string> &nonTerminals)
{
    std::unordered_map<std::string, std::vector<std::string>> result;

    for (int i = inputMap.size() - 1; i >= 0; i--)
    {
        const std::string &nonTerminal = inputMap.at(i).first;
        const std::set<std::vector<std::string>> &productions = inputMap.at(i).second;

        for (const std::vector<std::string> &production : productions)
        {
            bool containsEpsilon = true; // Assume all symbols have epsilon initially

            for (const std::string &symbol : production)
            {
                if (isNonTerminal(symbol, nonTerminals))
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

                        containsEpsilon &= (std::find(findResult->second.begin(), findResult->second.end(), "\\L") != findResult->second.end());
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
                                               { return isNonTerminal(symbol, nonTerminals) && result.at(symbol).empty(); }))
            {
                // Check for duplicates before inserting into the vector
                if (std::find(result[nonTerminal].begin(), result[nonTerminal].end(), "\\L") == result[nonTerminal].end())
                {

                    result[nonTerminal].push_back("\\L");
                }
            }
        }
    }

    return result;
}

std::vector<std::pair<std::string, std::string>> First_Follow::findAllOccurrences(
    const std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> &inputMap,
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

std::unordered_map<std::string, std::vector<std::string>> First_Follow::constructFollow(std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> inputMap, std::unordered_map<std::string, std::vector<std::string>> First, const set<string> &nonTerminals)
{
    std::unordered_map<std::string, std::vector<std::string>> result;
    // auto it = std::prev(inputMap.end());
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
            if (seenElements.find(nextElement) != seenElements.end())
            {
                // If seen, skip this occurrence
                continue;
            }
            if (isNonTerminal(nextElement, nonTerminals))
            {
                // If the next element is a non-terminal, append the First set of the next element to the result
                auto &firstSetOfNextElement = First[nextElement];
                result[currentKey].insert(result[currentKey].end(), firstSetOfNextElement.begin(), firstSetOfNextElement.end());
                // If the First set of the next element contains epsilon, exclude "Epsilon" and append the Follow set of the set key to the result
                auto epsilonFound = std::find(firstSetOfNextElement.begin(), firstSetOfNextElement.end(), "\\L");
                if (epsilonFound != firstSetOfNextElement.end())
                {
                    // Exclude "Epsilon" from the result
                    result[currentKey].erase(std::remove(result[currentKey].begin(), result[currentKey].end(), "\\L"), result[currentKey].end());

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
