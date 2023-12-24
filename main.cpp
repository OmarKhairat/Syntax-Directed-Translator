#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include "CFGParser/CFGParser.h"
#include <sstream>
#include "First Follow/First_Follow.h"
using namespace std;

int main()
{
    string projectPath = R"(C:\Users\abdel\Desktop\Connect-4\Syntax-Directed-Translator\)";

    // Test CFGParser
     std::unordered_map<std::string, std::set<std::vector<std::string>>> rules_map_set = CFGParser::get_CFG_rules(projectPath + "test1.txt");
/*
    // print rules_map
    for (auto &rule : rules_map_set)
    {
        cout << rule.first << " -> ";
        int i = 0;
        for (auto &rhs : rule.second)
        {
            for (auto &rhs_part : rhs)
            {
                cout << rhs_part << " ";
            }
            if(i != rule.second.size() - 1)
                cout << endl << "\t| ";
            else{
                cout << endl;
            }
            i++;
        }
    }

*/
/*
    std::unordered_map<std::string, std::set<std::vector<std::string>>> rules_map_set2;

     // Rule 1
    rules_map_set2["E"].insert({"T", "E'"});

    // Rule 2
    std::vector<std::string> rule2_vector1 = {"+", "T", "E'"};
    std::vector<std::string> rule2_vector2 = {"Epsilon"};
    rules_map_set2["E'"].insert(rule2_vector1);
    rules_map_set2["E'"].insert(rule2_vector2);

    // Rule 3
    rules_map_set2["T"].insert({"F", "T'"});

    // Rule 4
    std::vector<std::string> rule4_vector1 = {"*", "F", "T'"};
    std::vector<std::string> rule4_vector2 = {"Epsilon"};
    rules_map_set2["T'"].insert(rule4_vector1);
    rules_map_set2["T'"].insert(rule4_vector2);

    // Rule 5
    rules_map_set2["F"].insert({"(", "E", ")"});
    rules_map_set2["F"].insert({"'id'"});

     for (const auto& entry : rules_map_set2) {
        std::cout << entry.first << " -> ";
        for (const auto& vec : entry.second) {
            std::cout << "[ ";
            for (const auto& str : vec) {
                std::cout << str << " ";
            }
            std::cout << "] ";
        }
        std::cout << std::endl;
    }
*/
  std::unordered_map<std::string, std::set<std::vector<std::string>>> rules_map_set2;

    // METHOD_BODY = STATEMENT_LIST
    rules_map_set2["METHOD_BODY"].insert({{"STATEMENT_LIST"}});

    // STATEMENT_LIST = STATEMENT | STATEMENT_LIST STATEMENT
    rules_map_set2["STATEMENT_LIST"].insert({{"STATEMENT"}});
    rules_map_set2["STATEMENT_LIST"].insert({{"STATEMENT_LIST", "STATEMENT"}});

    // STATEMENT = DECLARATION | IF | WHILE | ASSIGNMENT
    rules_map_set2["STATEMENT"].insert({{"DECLARATION"}});
    rules_map_set2["STATEMENT"].insert({{"IF"}});
    rules_map_set2["STATEMENT"].insert({{"WHILE"}});
    rules_map_set2["STATEMENT"].insert({{"ASSIGNMENT"}});

    // DECLARATION = PRIMITIVE_TYPE 'id' ';'
    rules_map_set2["DECLARATION"].insert({{"PRIMITIVE_TYPE", "id", ";"}});

    // IF = 'if' '(' EXPRESSION ')' '{' STATEMENT '}'
    rules_map_set2["IF"].insert({{"if", "(", "EXPRESSION", ")", "{", "STATEMENT", "}"}});

    // WHILE = 'while' '(' EXPRESSION ')' '{' STATEMENT '}'
    rules_map_set2["WHILE"].insert({{"while", "(", "EXPRESSION", ")", "{", "STATEMENT", "}"}});

    // ASSIGNMENT = 'id' '=' EXPRESSION ';'
    rules_map_set2["ASSIGNMENT"].insert({{"id", "=", "EXPRESSION", ";"}});

    // EXPRESSION = SIMPLE_EXPRESSION | SIMPLE_EXPRESSION 'relop' SIMPLE_EXPRESSION
    rules_map_set2["EXPRESSION"].insert({{"SIMPLE_EXPRESSION"}});
    rules_map_set2["EXPRESSION"].insert({{"SIMPLE_EXPRESSION", "relop", "SIMPLE_EXPRESSION"}});

    // SIMPLE_EXPRESSION = TERM | SIGN TERM | SIMPLE_EXPRESSION 'addop' TERM
    rules_map_set2["SIMPLE_EXPRESSION"].insert({{"TERM"}});
    rules_map_set2["SIMPLE_EXPRESSION"].insert({{"SIGN", "TERM"}});
    rules_map_set2["SIMPLE_EXPRESSION"].insert({{"SIMPLE_EXPRESSION", "addop", "TERM"}});

    // TERM = FACTOR | TERM 'mulop' FACTOR
    rules_map_set2["TERM"].insert({{"FACTOR"}});
    rules_map_set2["TERM"].insert({{"TERM", "mulop", "FACTOR"}});

    // FACTOR = 'id' | 'num' | '(' EXPRESSION ')'
    rules_map_set2["FACTOR"].insert({{"id"}});
    rules_map_set2["FACTOR"].insert({{"num"}});
    rules_map_set2["FACTOR"].insert({{"(", "EXPRESSION", ")"}});

    // SIGN = '+' | '-'
    rules_map_set2["SIGN"].insert({{"+"}});
    rules_map_set2["SIGN"].insert({{"-"}});

    // Display the contents of the map
    for (const auto& entry : rules_map_set2) {
        std::cout << entry.first << " -> ";
        for (const auto& vec : entry.second) {
            std::cout << "[ ";
            for (const auto& str : vec) {
                std::cout << str << " ";
            }
            std::cout << "] ";
        }
        std::cout << std::endl;
    }
    First_Follow firstFollowObject;
    std::unordered_map<std::string, std::vector<std::string>> First = firstFollowObject.constructFirst(rules_map_set2);
    for (const auto &entry : First) {
        const std::string &key = entry.first;
        const std::vector<std::string> &value = entry.second;

        std::cout << "Key: " << key << "\nValues:";
        for (const std::string &str : value) {
            std::cout << " " << str;
        }
        std::cout << "\n\n";
    }

/*
   std::unordered_map<std::string, std::set<std::vector<std::string>>> rules_map_set3;

    rules_map_set3["F"].insert({"(", "E", ")"});
    rules_map_set3["F"].insert({"'id'"});
     // Rule 1
      std::vector<std::string> rule11_vector1 = {"*", "F", "T'"};
    std::vector<std::string> rule111_vector2 = {"Epsilon"};
    rules_map_set3["T'"].insert(rule11_vector1);
    rules_map_set3["T'"].insert(rule111_vector2);
       rules_map_set3["T"].insert({"F", "T'"});
       std::vector<std::string> rule22_vector1 = {"+", "T", "E'"};
    std::vector<std::string> rule222_vector2 = {"Epsilon"};
    rules_map_set3["E'"].insert(rule22_vector1);
    rules_map_set3["E'"].insert(rule222_vector2);
    rules_map_set3["E"].insert({"T", "E'"});


*/

    // Rule 5

/*
     for (const auto& entry : rules_map_set3) {
        std::cout << entry.first << " -> ";
        for (const auto& vec : entry.second) {
            std::cout << "[ ";
            for (const auto& str : vec) {
                std::cout << str << " ";
            }
            std::cout << "] ";
        }
        std::cout << std::endl;
    }
    std::unordered_map<std::string, std::vector<std::string>> Follow = firstFollowObject.constructFollow(rules_map_set3,First);

     for (const auto &entry : Follow) {
        const std::string &key = entry.first;
        const std::vector<std::string> &values = entry.second;

        std::cout << "Key: " << key << ", Values: ";
        for (const auto &value : values) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }*/
    /*
    LexicalAnalyzerFactory factory(projectPath);
    LexicalAnalyzer lexicalAnalyzer = factory.getLexicalAnalyzer();

    string testProgram = "int sum , count , pass , mnt; while (pass !=\n"
                         "10)\n"
                         "{\n"
                         "pass = pass + 1 ;\n"
                         "}";

    lexicalAnalyzer.setExpression(testProgram);
    while (lexicalAnalyzer.hasNextToken())
    {
        pair<string, string> token = lexicalAnalyzer.getNextToken();
        cout << "TOKEN = " << token.first << " --- MATCHED PATTERN = " << token.second << endl;
    }

    string testString = "int n = 3\n"
                        "float f = 56.7;\n"
                        "float f2 = 5.67E1\n"
                        "x x,x 5 n \n"
                        "if (f >50) { f = f2 / 2}\n"
                        "else { f = f2 * 2}";

    lexicalAnalyzer.setExpression(testString);
    while (lexicalAnalyzer.hasNextToken())
    {
        pair<string, string> token = lexicalAnalyzer.getNextToken();
        cout << "TOKEN = " << token.first << " --- MATCHED PATTERN = " << token.second << endl;
    }

    string test2 = "int x = 70 e b);";

    lexicalAnalyzer.setExpression(test2);
    while (lexicalAnalyzer.hasNextToken())
    {
        pair<string, string> token = lexicalAnalyzer.getNextToken();
        cout << "TOKEN = " << token.first << " --- MATCHED PATTERN = " << token.second << endl;
    }

    string test3 = "int x = 70&y;";

    lexicalAnalyzer.setExpression(test3);
    while (lexicalAnalyzer.hasNextToken())
    {
        pair<string, string> token = lexicalAnalyzer.getNextToken();
        cout << "TOKEN = " << token.first << " --- MATCHED PATTERN = " << token.second << endl;
    }

    string test4 = "boolean x = 0\n"
                   "boolean x = false ";

    lexicalAnalyzer.setExpression(test4);
    while (lexicalAnalyzer.hasNextToken())
    {
        pair<string, string> token = lexicalAnalyzer.getNextToken();
        cout << "TOKEN = " << token.first << " --- MATCHED PATTERN = " << token.second << endl;
    }
    */

    return 0;
}
