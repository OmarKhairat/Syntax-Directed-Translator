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

std::vector<std::pair<std::string, std::vector<std::string>>>  First_Follow::findAllOccurrences(
    const std::vector<std::pair<std::string, std::set<std::vector<std::string>>>> &inputMap,
    const std::string &key)
{
/*
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
    */

    std::vector<std::pair<std::string, std::vector<std::string>>> occurrences;

    for (const auto &entry : inputMap)
    {
        const std::set<std::vector<std::string>> &currentSet = entry.second;

        // Check if the key is present in the current set
        for (const auto &vectorOfString : currentSet)
        {
            auto keyIterator = std::find(vectorOfString.begin(), vectorOfString.end(), key);

            if (keyIterator != vectorOfString.end())
            {
                // Key found in the current set, retrieve elements after it until the end of the vector
                auto start = std::next(keyIterator);
                std::vector<std::string> elementsAfterKey(start, vectorOfString.end());

                // Add the pair to the occurrences vector
                std::string setKey = entry.first;
                occurrences.push_back(std::make_pair(setKey, elementsAfterKey));
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
        for (const auto &occurrence : occurrences)
{
    // Extract information from the occurrence
    const std::string &setKey = occurrence.first;
    const std::vector<std::string> &elementsAfterKey = occurrence.second;

        const std::string &nextElement = elementsAfterKey.front();

            if(elementsAfterKey.empty())
            {
                result[currentKey].insert(result[currentKey].end(), result[setKey].begin(), result[setKey].end());
            }
        // Call the isNonTerminal function
       else if (isNonTerminal(nextElement, nonTerminals))
        {

            // If it is a non-terminal, get its first set from the first map
            const std::vector<std::string> &firstSet = First[nextElement];
            // Check if the first set contains "\\L"
            auto itL = std::find(firstSet.begin(), firstSet.end(), "\\L");
            if (itL != firstSet.end())
            {
                // If it contains "\\L," exclude it from the values to be pushed
                result[currentKey].insert(result[currentKey].end(), firstSet.begin(),firstSet.end());
                    // Remove "\\L" from currentKey
                    result[currentKey].erase(
               std::remove(result[currentKey].begin(), result[currentKey].end(), "\\L"), result[currentKey].end());

                // Check if there are no elements after "\\L" in elementsAfterKey
                if (elementsAfterKey.size()==1)
                {
                    result[currentKey].insert(result[currentKey].end(), result[setKey].begin(), result[setKey].end());
                    continue;
                }
                bool flag = false;
                // Loop on the rest of elementsAfterKey
               auto itL2 = elementsAfterKey.begin();
                for (auto it = std::next(itL2); it != elementsAfterKey.end(); ++it)
                {

                    // Process each element after "\\L"
                    if (isNonTerminal(*it, nonTerminals))
                    {
                        // If the element is a non-terminal, you can add your logic here
                        const std::vector<std::string> &nonTerminalFirstSet = First[*it];
                        auto itL = std::find(nonTerminalFirstSet.begin(), nonTerminalFirstSet.end(), "\\L");
                        if(itL != nonTerminalFirstSet.end())
                        {
                             result[currentKey].insert(result[currentKey].end(), nonTerminalFirstSet.begin(), nonTerminalFirstSet.end());

                    result[currentKey].erase(
                     std::remove(result[currentKey].begin(), result[currentKey].end(), "\\L"), result[currentKey].end());
                           flag = true;

                        }
                        else
                        {
                             result[currentKey].insert(result[currentKey].end(), nonTerminalFirstSet.begin(), nonTerminalFirstSet.end());
                             flag = false;
                            break;
                        }

                    }
                    else
                    {
                        // If the element is a terminal, push it and break the loop
                        result[currentKey].push_back(*it);
                        break;
                    }
                }
                if(flag)
                {
                    result[currentKey].insert(result[currentKey].end(), result[setKey].begin(), result[setKey].end());
                    flag = false;
                }
            }
            else
            {
                // If it doesn't contain "\\L," push all values to the result vector
                result[currentKey].insert(result[currentKey].end(), firstSet.begin(), firstSet.end());
            }
        }
        else
        {
             // If the next element is a terminal, append it to the result
            result[currentKey].push_back(nextElement);
        }
    // Handle the case where elementsAfterKey is empty if needed
}
        /*
        for (const auto &occurrence : occurrences)
        {
            const std::string &nextElement = occurrence.first;
            const std::string &setKey = occurrence.second;
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
        */
    }
    for (auto &entry : result) {
    auto &followSet = entry.second;

    // Sort the vector to bring duplicates together
    std::sort(followSet.begin(), followSet.end());

    // Use std::unique to remove consecutive duplicates
    auto uniqueEnd = std::unique(followSet.begin(), followSet.end());

    // Erase the duplicates from the vector
    followSet.erase(uniqueEnd, followSet.end());
    }

    return result;
}
