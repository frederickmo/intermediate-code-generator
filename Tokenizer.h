//
// Created by Frederick Mo on 2021/12/31.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_TOKENIZER_H
#define INTERMEDIATE_CODE_GENERATOR_TOKENIZER_H

#include "Keywords.h"

#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

class Word;


// 词法分析表
vector<Word> lexicalTable;
int lexicalTableLength = 0;

// 定位二元式在源代码哪一行
vector<int> locateInputCode;


static bool isSmallLetter(char c) {
    return c >= 'a' && c <= 'z';
}

static bool isCapitalLetter(char c) {
    return c >= 'A' && c <= 'Z';
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isUnderline(char c) {
    return c == '_';
}

static bool isDecimalPoint(char c) {
    return c == '.';
}

static bool isBlank(char c) {
    return c == ' ';
}

class Word {
public:
    int indexInKeywords{};
    string token;

    Word() = default;

    void print() const;
};


class Tokenizer {
public:
    int _length;
    vector<int> locationOfBinaryTermInInputFile;

    // 分析关键字/用户定义变量
    static Word wordAnalyze(const string &subCode);

    // 识别数字
    static Word numberAnalyze(const string &subCode);

    // 识别字符
    static Word characterAnalyze(const string &subCode);

    static void lexicalAnalyze(const string &code);
};


void Word::print() const {
    cout << "indexInKeyWords => " << indexInKeywords << ", token => " << token << endl;
}


Word Tokenizer::wordAnalyze(const string &subCode) {
    Word word;
    bool isKeyword = false;
    for (int i = CHARACTER_COUNT; i < TOTAL_KEYWORD_COUNT; ++i) {
        if (subCode.substr(0, keywords[i].length()) == keywords[i]) {
            word.indexInKeywords = i;
            isKeyword = true;
        }
    }
    if (!isKeyword) {
        // 非关键字视为变量名
        for (int i = 0; i < subCode.length(); ++i) {
            // 找到第一个不是数字/字母/下划线的位置
            if (!(isLetter(subCode[i]) || isDigit(subCode[i]) || isUnderline(subCode[i]))) {
                // VARIABLE(1) --- 是变量
                word.indexInKeywords = VARIABLE;
                keywords[word.indexInKeywords] = subCode.substr(0, i);
                break;
            } else if ((isLetter(subCode[i]) || isDigit(subCode[i]) || isUnderline(subCode[i])) &&
                       i == subCode.length() - 1) {
                word.indexInKeywords = VARIABLE;
                keywords[word.indexInKeywords] = subCode.substr(0, subCode.length());
                break;
            }
        }
    }
    word.token = keywords[word.indexInKeywords];
    return word;
}


Word Tokenizer::numberAnalyze(const string &subCode) {
    Word word;
    for (int i = 0; i <= subCode.length(); ++i) {
        // 既不是数字也不是小数点，
        // 截取到第一个非数字和小数点的字符
        if (!(isDigit(subCode[i]) || isDecimalPoint(subCode[i]))) {
            string tempNumber = subCode.substr(0, i);
            // 没读到'.'则返回很大的数，若读到则返回第一次出现的下标
            if (tempNumber.find('.') == string::npos)
                word.indexInKeywords = INTEGER;
            else
                word.indexInKeywords = REAL;
            keywords[word.indexInKeywords] = tempNumber;
            break;
        }
    }
    word.token = keywords[word.indexInKeywords];
    return word;
}


Word Tokenizer::characterAnalyze(const string &subCode) {
    Word word;
    switch (subCode[0]) {
        case '#':
            word.indexInKeywords = 0;
            break;
        case '+':
            word.indexInKeywords = 5;
            break;
        case '-':
            word.indexInKeywords = 6;
            break;
        case '*':
            word.indexInKeywords = 7;
            break;
        case '/':
            word.indexInKeywords = 8;
            break;
        case '<':
            if (subCode[1] == '=')
                word.indexInKeywords = 10;
            else
                word.indexInKeywords = 9;
            break;
        case '=':
            if (subCode[1] == '=')
                word.indexInKeywords = 11;
            else
                word.indexInKeywords = 18;
            break;
        case '!':
            if (subCode[1] == '=')
                word.indexInKeywords = 12;
            else
                word.indexInKeywords = 30;
            break;
        case '>':
            if (subCode[1] == '=')
                word.indexInKeywords = 13;
            else
                word.indexInKeywords = 14;
            break;
        case '&':
            if (subCode[1] == '&')
                word.indexInKeywords = 16;
            else
                word.indexInKeywords = 15;
        case '|':
            if (subCode[1] == '|')
                word.indexInKeywords = 17;
            break;
        case '(':
            word.indexInKeywords = 19;
            break;
        case ')':
            word.indexInKeywords = 20;
            break;
        case '[':
            word.indexInKeywords = 21;
            break;
        case ']':
            word.indexInKeywords = 22;
            break;
        case '{':
            word.indexInKeywords = 23;
            break;
        case '}':
            word.indexInKeywords = 24;
            break;
        case ':':
            if (subCode[1] == '=')
                word.indexInKeywords = 26;
            else
                word.indexInKeywords = 25;
            break;
        case ';':
            word.indexInKeywords = 27;
            break;
        case ',':
            word.indexInKeywords = 28;
            break;
        case '@':
            word.indexInKeywords = 29;
            break;
    }
    word.token = keywords[word.indexInKeywords];
    return word;
}


void Tokenizer::lexicalAnalyze(const string &code) {
    int length = code.length();
    for (int i = 0; i < code.length(); ++i) {
        Word word;
//        cout << "isLetter:" << isLetter(code[i]) << "isDigit:" << isDigit(code[i]) << "isBlank:" << isBlank(code[i]) << endl;
        if (isLetter(code[i]))
            word = wordAnalyze(code.substr(i, code.length() - i + 1));
        else if (isDigit(code[i]))
            word = numberAnalyze(code.substr(i, code.length() - i + 1));
        else if (isBlank(code[i]))
            continue;
        else
            word = characterAnalyze(code.substr(i, code.length() - i + 1));

        i += int(word.token.length()) - 1;
        lexicalTable.emplace_back(word);
        ++lexicalTableLength;
//        cout << "(" << word.indexInKeywords << ", " << word.token << ")" << endl;
    }
}


#endif //INTERMEDIATE_CODE_GENERATOR_TOKENIZER_H
