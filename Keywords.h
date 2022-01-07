//
// Created by Frederick Mo on 2021/12/31.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_KEYWORDS_H
#define INTERMEDIATE_CODE_GENERATOR_KEYWORDS_H

#include <vector>
#include <string>

using std::vector;
using std::string;

#define CHARACTER_COUNT 31
#define RESERVED_WORD_COUNT 11
#define TOTAL_KEYWORD_COUNT 42

// 以下是变量的在关键字表里的位置
#define VARIABLE 1
#define INTEGER 2
#define REAL 3
#define CONSTANT 4


vector<string> keywords = {
        "#",             // 0
        "variable_name",
        "integer",
        "real",
        "constant",
        "+",             // 5
        "-",             // 6
        "*",             // 7
        "/",             // 8
        "<",             // 9
        "<=",            // 10
        "==",            // 11
        "!=",            // 12
        ">=",            // 13
        ">",             // 14
        "&",             // 15
        "&&",            // 16
        "||",            // 17
        "=",             // 18
        "(",             // 19
        ")",             // 20
        "[",             // 21
        "]",             // 22
        "{",             // 23
        "}",             // 24
        ":",             // 25
        ":=",            // 26
        ";",             // 27
        ",",             // 28
        "@",             // 29
        "!",             // 30
        "if",
        "then",
        "else",
        "while",
        "do",
        "null",
        "and",
        "or",
        "not",
        "call",
        "goto",
};

const int characterCount = 31;
const int reservedWordsCount = 11;
const int totalKeywordsCount = 42;

#endif //INTERMEDIATE_CODE_GENERATOR_KEYWORDS_H
