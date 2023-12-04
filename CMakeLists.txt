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
        patternmatcher/PatternMatcher.cpp
        patternmatcher/PatternMatcher.h
        main.cpp)
