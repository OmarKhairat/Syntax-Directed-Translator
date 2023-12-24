#include "TopDownParser.h"

TopDownParser::TopDownParser(unordered_map<string, unordered_map<string, vector<string>>> table)
{
    TopDownParser::table = table;
}