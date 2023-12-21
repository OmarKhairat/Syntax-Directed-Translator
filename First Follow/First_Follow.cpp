#include "First_Follow.h"

std::unordered_map<std::string, std::vector<std::string>> First_Follow::constructFirst(std::unordered_map<std::string, std::set<std::vector<std::string>>> inputMap)
{
    std::unordered_map<std::string, std::vector<std::string>> result;

    // Loop through the inputMap in reverse order
    for (auto it = inputMap.rbegin(); it != inputMap.rend(); ++it)
    {
        const std::string &nonTerminal = it->first;
        const std::set<std::vector<std::string>> &productions = it->second;

        // Loop through the vectors of the current non-terminal
        for (const std::vector<std::string> &production : productions)
        {
            bool containsEpsilon = false;

            // Process each symbol in the production
            for (const std::string &symbol : production)
            {
                if (isNonTerminal(symbol))
                {
                    // If the symbol is a non-terminal, find its First set in the result map
                    auto findResult = result.find(symbol);
                    if (findResult != result.end())
                    {
                        // Append the First set of the non-terminal to the result
                        result[nonTerminal].insert(result[nonTerminal].end(), findResult->second.begin(), findResult->second.end());

                        // Check if the First set of the non-terminal contains epsilon
                        if (std::find(findResult->second.begin(), findResult->second.end(), "Epsilon") != findResult->second.end())
                        {
                            containsEpsilon = true;
                        }
                        else
                        {
                            // If it doesn't contain epsilon, break the loop
                            break;
                        }
                    }
                }
                else
                {
                    // The symbol is a terminal, add it to the result and break the loop
                    result[nonTerminal].push_back(symbol);
                    break;
                }
            }

            // If the whole vector is non-terminal and all non-terminals have epsilon in their First set, push epsilon
            if (!containsEpsilon && std::all_of(production.begin(), production.end(), [this](const std::string &symbol)
                                                { return isNonTerminal(symbol) && result[symbol].empty(); }))
            {
                result[nonTerminal].push_back("Epsilon");
            }
        }
    }

    return result;
}

bool isNonTerminal(const std::string &symbol)
{
    // Check if the symbol is all uppercase or all uppercase with underscores and not equal to "Epsilon"
    return (std::all_of(symbol.begin(), symbol.end(), ::isupper) ||
            (std::all_of(symbol.begin(), symbol.end(), ::isupper) &&
             std::any_of(symbol.begin(), symbol.end(), [](char c)
                         { return c == '_'; }))) &&
           (symbol != "Epsilon");
}

std::unordered_map<std::string, std::vector<std::string>> First_Follow::constructFollow(std::unordered_map<std::string, std::set<std::vector<std::string>>> inputMap, std::unordered_map<std::string, std::vector<std::string>> First)
{
    std::unordered_map<std::string, std::vector<std::string>> result;
    auto it = inputMap.begin();
    const std::string &firstKey = it->first;
    result[firstKey].push_back("$");
    for (const auto &entry : inputMap)
    {
        const std::string &currentKey = entry.first;

        auto occurrences = findAllOccurrences(inputMap, currentKey);

        for (const auto &occurrence : occurrences)
        {
            const std::string &nextElement = occurrence.first;
            const std::string &setKey = occurrence.second;

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
            }
            else if (nextElement == "")
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

std::vector<std::pair<std::string, std::string>> findAllOccurrences(
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