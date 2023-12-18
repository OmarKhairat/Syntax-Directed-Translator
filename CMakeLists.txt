cmake_minimum_required(VERSION 3.26)
project(Syntax_Directed_Translator)

add_executable(Syntax_Directed_Translator
        Parser/parser.cpp
        Parser/parser.h
        PatternMatcher/PatternMatcher.cpp
        PatternMatcher/PatternMatcher.h
        main.cpp
        NFA/NFA.h
        NFA/NFA.cpp
        DFA/DFA.h
        DFA/DFA.cpp)
