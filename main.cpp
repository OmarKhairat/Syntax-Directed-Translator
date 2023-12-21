#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include "CFGParser/CFGParser.h"
#include <sstream>

using namespace std;

int main()
{
    string projectPath = R"(D:\E\Collage\Year_4_1\Compilers\Project\Syntax-Directed-Translator\)";

    // Test CFGParser
    unordered_map< string, set< vector<string> > > rules_map_set = CFGParser::get_CFG_rules(projectPath + "CFG_rules.txt");

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
