cmake_minimum_required(VERSION 3.26)
project(Syntax_Directed_Translator)

add_executable(Syntax_Directed_Translator
        main.cpp
        Parser/parser.cpp
        Parser/parser.h
        PatternMatcher/PatternMatcher.cpp
        PatternMatcher/PatternMatcher.h
        NFA/NFA.h
        NFA/NFA.cpp
        DFA/DFA.h
        DFA/DFA.cpp
        CFGParser/CFGParser.h
        CFGParser/CFGParser.cpp
        FirstFollow/First_Follow.h
        FirstFollow/First_Follow.cpp
        ParsingTable/ParsingTable.h
        ParsingTable/ParsingTable.cpp
        )
