cmake_minimum_required(VERSION 3.20)
project(intermediate_code_generator)

set(CMAKE_CXX_STANDARD 14)

add_executable(intermediate_code_generator main.cpp parsingTable.h Tokenizer.h Keywords.h GrammarAnalyzer.h SemanticAnalyzer.h QuadrupleTranslator.h)
