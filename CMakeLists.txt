cmake_minimum_required(VERSION 3.26)
project(Syntax_Directed_Translator)

set(CMAKE_CXX_STANDARD 14)

include_directories(LexicalAnalyzer)
include_directories(Parser)

add_executable(Syntax_Directed_Translator
        LexicalAnalyzer/lexicalAnalyzer.cpp
        LexicalAnalyzer/lexicalAnalyzer.h
        Parser/parser.cpp
        Parser/parser.h
        PatternMatcher/PatternMatcher.cpp
        PatternMatcher/PatternMatcher.h
        main.cpp
        NFA/NFA.h
        NFA/NFA.cpp
        DFA/DFA.h
        DFA/DFA.cpp)
