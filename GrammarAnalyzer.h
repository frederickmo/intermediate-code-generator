//
// Created by Frederick Mo on 2022/1/1.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_GRAMMARANALYZER_H
#define INTERMEDIATE_CODE_GENERATOR_GRAMMARANALYZER_H

#include "Tokenizer.h"

#include <map>
#include <set>
#include <algorithm>

using std::map;
using std::set;

// 将非终结符和终结符映射到唯一的索引的map
static map<string, int> VnToIndexMap, VtToIndexMap;

const vector<string> productionTable = {

/*0*/            "<<ACC>>-><<START>>",

/*1*/            "A-><<ID>>:=<<EXPR>>",

/*2*/            "<<BAND>>-><<BNOT>>",
/*3*/            "<<BAND>>-><<BAND>>andM<<BNOT>>",

/*4*/            "<<BCMP>>-><<EXPR>><<RELOP>><<EXPR>>",
/*5*/            "<<BCMP>>-><<EXPR>>",
/*6*/            "<<BCMP>>->(<<BEXPR>>)",

/*7*/            "<<BEXPR>>-><<BAND>>",
/*8*/            "<<BEXPR>>-><<BEXPR>>orM<<BAND>>",

/*9*/            "<<BNOT>>-><<BCMP>>",
/*10*/            "<<BNOT>>->not<<BNOT>>",

/*11*/            "<<ELIST>>-><<EXPR>>",
/*12*/            "<<ELIST>>-><<ELIST>>,<<EXPR>>",

/*13*/            "<<EXPR>>-><<NEG>>",
/*14*/            "<<EXPR>>-><<EXPR>>+<<TERM>>",

/*15*/            "<<FACTOR>>-><<ID>>",
/*16*/            "<<FACTOR>>->(<<EXPR>>)",

/*17*/            "<<ID>>->i", // variable_name

/*18*/            "L->L;MS",
/*19*/            "L->S",

/*20*/            "<<LABEL>>-><<ID>>:",

/*21*/            "M->null",
/*22*/            "N->null",

/*23*/            "<<NEG>>-><<TERM>>",
/*24*/            "<<NEG>>->-<<TERM>>",

/*25*/            "<<OPENSTMT>>->if<<BEXPR>>thenM<<STMT>>",
/*26*/            "<<OPENSTMT>>->if<<BEXPR>>thenMSNelseM<<OPENSTMT>>",

/*27*/            "<<RELOP>>->=",
/*28*/            "<<RELOP>>->!=",
/*29*/            "<<RELOP>>-><",
/*30*/            "<<RELOP>>-><=",
/*31*/            "<<RELOP>>->>",
/*32*/            "<<RELOP>>->>=",

/*33*/            "S->if<<BEXPR>>thenMSNelseMS",
/*34*/            "S->A",
/*35*/            "S->{L}",
/*36*/            "S->whileM<<BEXPR>>doMS",
/*37*/            "S->call<<ID>>(<<ELIST>>)",
/*38*/            "S-><<LABEL>>S",
/*39*/            "S->goto<<ID>>",

/*40*/            "<<START>>-><<STMT>>",

/*41*/            "<<STMT>>->S",
/*42*/            "<<STMT>>-><<OPENSTMT>>",

/*43*/            "<<TERM>>-><<FACTOR>>",
/*44*/            "<<TERM>>-><<TERM>>*<<FACTOR>>"

};

int symbolCount = 0;
map<string, bool> nullable;
// 所有终结符/非终结符的FIRST集合
// => 终结符的FIRST集合为自己
set<string> FIRST[60];
// 所有终结符/非终结符的FOLLOW集合
// => 终结符的FOLLOW集合为空
set<string> FOLLOW[60];


class GrammarAnalyzer {
public:

    static string getVn(const string& production);
    static string getVt(const string& production);
    // 对产生式进行扫描，识别所有终结符和非终结符
    static void scanProduction();
    // RHS = right hand side 将产生式右部按（非）终结符分割
    static vector<string> splitRHSOfProduction(const string& RHSOfProduction);

    // 将产生式分割为左部和右部
    static void split(const string& production, string &LHS, vector<string>& RHS);

    // 判断从leftIndex到rightIndex之间的符号是否可推导为空字
    static bool allNullable(vector<string> Yset, int leftIndex, int rightIndex);

    // 计算一次所有符号的FIRST集合和FOLLOW集合
    static void calculateFIRSTAndFOLLOWSets();

    // 计算得到最终FIRST和FOLLOW集合
    static void calculateFinalFIRSTAndFOLLOWSets();



};

string GrammarAnalyzer::getVn(const string &production) {
    if (production.substr(0, 2) == "<<") {
        // 处理<< expr >> 格式的非终结符
        // ↓ 找到 >> 的位置
        int bracketsDelimiter = production.find(">>");
        string Vn = production.substr(0, bracketsDelimiter + 2);
        return Vn;
    } else if (isDigit(production[1])) {
        // 带序号的非终结符，如E1, T2
        return production.substr(0, 2);
    } else {
        return production.substr(0, 1);
    }
}

string GrammarAnalyzer::getVt(const string &production) {
    for (int i = 0; i < CHARACTER_COUNT; ++i) {
        // 最长匹配，优先匹配两个字符的终结符（字符）
        if (production.substr(0, 2) == keywords[i])
            return production.substr(0, 2);
    }
    for (int i = 0; i < CHARACTER_COUNT; ++i) {
        // 其次匹配一个字符的终结符
        if (production.substr(0, 1) == keywords[i])
            return production.substr(0, 1);
    }
    for (int i = CHARACTER_COUNT; i < TOTAL_KEYWORD_COUNT; ++i) {
        // 然后匹配保留字
        string Vt = production.substr(0, keywords[i].length());
        if (Vt == keywords[i])
            return Vt;
    }
    // 关键字表里都没有匹配，则认为小写字母也是终结符
    if (isSmallLetter(production[0]))
        return production.substr(0, 1);
}

void GrammarAnalyzer::scanProduction() {
    for (auto &production: productionTable) {
        for (int i = 0; i < production.length(); ++i) {
            // 为空：不识别
            if (isBlank(production[i]))
                continue;
            else if (production.substr(i, 2) == "<<") {
                // 处理<<expr>>格式的非终结符(串)
                int bracketsDelimiter = production.substr(i, production.length() - i).find(">>");
                string Vn = production.substr(i, bracketsDelimiter + 2);
                if (VnToIndexMap[Vn] == 0) {
                    // 该非终结符在映射map里未出现过
                    // 所有符号数+1，存入映射map中
                    VnToIndexMap[Vn] = ++symbolCount;
                }
                i += Vn.length() - 1;
            } else if (isCapitalLetter(production[i])) {
                // 大写字母识别为非终结符
                string Vn = getVn(production.substr(i, 2));
                if (VnToIndexMap[Vn] == 0)
                    VnToIndexMap[Vn] = ++symbolCount;
                i += Vn.length() - 1;
            } else if (production.substr(i, 2) == "->") {
                // 分割产生式左部和产生式右部的符号，略过
                i += 1;
            } else {
                // 识别产生式右部可能的终结符
                string Vt = getVt(production.substr(i, production.length() - i));
                // 终结符第一次出现，映射为出现的序号（下标索引）
                if (VtToIndexMap[Vt] == 0)
                    VtToIndexMap[Vt] = ++symbolCount;
                i += Vt.length() - 1;
            }
        }
    }
}


    vector<string> GrammarAnalyzer::splitRHSOfProduction(const string &RHSOfProduction) {
        vector<string> Ysets;
        for (int i = 0; i < RHSOfProduction.length(); ++i) {
            if (isBlank(RHSOfProduction[i]))
                continue;
            else if (isCapitalLetter(RHSOfProduction[i]) || RHSOfProduction.substr(i, 2) == "<<") {
                // ①该符号首字符是大写字母 ②以"<<"串开头 是非终结符
                string Vn = getVn(RHSOfProduction.substr(i, RHSOfProduction.length() - i));
                Ysets.push_back(Vn);
                i += Vn.length() - 1;
            } else {
                // 除此之外是终结符
                string Vt = getVt(RHSOfProduction.substr(i, RHSOfProduction.length() - i));
                Ysets.push_back(Vt);
                i += Vt.length() - 1;
            }
        }
        return Ysets;
    }

    void GrammarAnalyzer::split(const string &production, string &LHS, vector<string> &RHS) {
        int delimiterIndex = production.find("->");
        LHS = production.substr(0, delimiterIndex);
        // 实现trim()函数功能：移除前后空格
        LHS.erase(0, LHS.find_first_not_of(' '));
        LHS.erase(LHS.find_last_not_of(' ') + 1);
        string rightProduction = production.substr(delimiterIndex + 2, production.length() - delimiterIndex - 2);
        RHS = splitRHSOfProduction(rightProduction);
    }

    bool GrammarAnalyzer::allNullable(vector<string> Yset, int leftIndex, int rightIndex) {
        if (leftIndex >= Yset.size() || leftIndex > rightIndex || rightIndex < 0)
            return true;
        for (int i = leftIndex; i <= rightIndex; ++i) {
            // 任何一个字符不可推导为空字即返回false
            if (!nullable[Yset[i]])
                return false;
        }
        return true;
    }

    void GrammarAnalyzer::calculateFIRSTAndFOLLOWSets() {
        for (auto const &i: VtToIndexMap) {
            string Vt = i.first;
            int VtIndex = i.second;
            // 对于终结符：FIRST集合即为自己
            FIRST[VtIndex].insert(Vt);
        }
        for (const auto &production: productionTable) {
            string X;
            vector<string> Yset;
            int delimiterIndex = production.find("->");
            X = production.substr(0, delimiterIndex);
            X.erase(0, X.find_first_not_of(' '));
            X.erase(X.find_last_not_of(' ') + 1);
            string rightProduction = production.substr(
                    delimiterIndex + 2,
                    production.length() - delimiterIndex - 2);
            Yset = splitRHSOfProduction(rightProduction);

            int sizeOfYset = Yset.size();
            nullable["null"] = true;
            if (allNullable(Yset, 0, sizeOfYset - 1))
                nullable[X] = true;

            for (int i = 0; i < sizeOfYset; ++i) {

                // ① 如果前i-1个符号都可空而Yi不可空，则将FIRST(Yi)集合加入FIRST(X)中
                if ((!nullable[Yset[i]]) && allNullable(Yset, 0, i - 1)) {
                    if (i <= sizeOfYset - 1) {
                        set<string> FIRSTSetOfX = FIRST[VnToIndexMap[X]];
                        // 判断Yi是否是终结符 => 按符号表中索引获取FIRST集合
                        set<string> FIRSTSetOfYi =
                                VtToIndexMap.count(Yset[i]) != 0 ?
                                FIRST[VtToIndexMap[Yset[i]]] : FIRST[VnToIndexMap[Yset[i]]];
                        // 取并集操作
                        std::set_union(FIRSTSetOfX.begin(), FIRSTSetOfX.end(),
                                       FIRSTSetOfYi.begin(), FIRSTSetOfYi.end(),
                                       std::inserter(FIRSTSetOfX, FIRSTSetOfX.begin()));
                        // 更新X的FIRST集合
                        FIRST[VnToIndexMap[X]] = FIRSTSetOfX;
                    }
                }

                // ② 如果Y(i+1)到Y(sizeOfYset-1)都是可空的而Yi之前的不可空，则将FOLLOW(X)加入FOLLOW(Yi)中
                if (allNullable(Yset, i + 1, sizeOfYset - 1)) {
                    set<string> FOLLOWSetOfX = FOLLOW[VnToIndexMap[X]];
                    set<string> FOLLOWSetOfYi =
                            VtToIndexMap.count(Yset[i]) ?
                            FOLLOW[VtToIndexMap[Yset[i]]] : FOLLOW[VnToIndexMap[Yset[i]]];
                    std::set_union(FOLLOWSetOfX.begin(), FOLLOWSetOfX.end(),
                                   FOLLOWSetOfYi.begin(), FOLLOWSetOfYi.end(),
                                   std::inserter(FOLLOWSetOfYi, FOLLOWSetOfYi.begin()));
                    VtToIndexMap.count(Yset[i]) ?
                    FOLLOW[VtToIndexMap[Yset[i]]] : FOLLOW[VnToIndexMap[Yset[i]]]
                                                            = FOLLOWSetOfYi;
                }

                for (int j = i + 1; j < sizeOfYset; ++j) {
                    // ③ 如果Y(i+1)到Y(j-1)都可推导为空字，则将FIRST(Yj)加入FOLLOW(Yi)中
                    if ((!nullable[Yset[j]]) && allNullable(Yset, i + 1, j - 1) && j <= sizeOfYset - 1) {
                        set<string> FOLLOWSetOfYi =
                                VtToIndexMap.count(Yset[i]) ?
                                FOLLOW[VtToIndexMap[Yset[i]]] : FOLLOW[VnToIndexMap[Yset[i]]];
                        set<string> FIRSTSetOfYj =
                                VtToIndexMap.count(Yset[j]) ?
                                FIRST[VtToIndexMap[Yset[j]]] : FIRST[VnToIndexMap[Yset[j]]];
                        std::set_union(FOLLOWSetOfYi.begin(), FOLLOWSetOfYi.end(),
                                       FIRSTSetOfYj.begin(), FIRSTSetOfYj.end(),
                                       std::inserter(FOLLOWSetOfYi, FOLLOWSetOfYi.begin()));

                        VtToIndexMap.count(Yset[i]) ?
                        FOLLOW[VtToIndexMap[Yset[i]]] : FOLLOW[VnToIndexMap[Yset[i]]]
                                                                = FOLLOWSetOfYi;
                    }
                }
            }
        }
    }

    void GrammarAnalyzer::calculateFinalFIRSTAndFOLLOWSets() {
        set<string> previousFIRST[60];
        set<string> previousFOLLOW[60];
        // FIRST集合和FOLLOW集合是否已收敛（不再变化）？
        bool isConverged = true;
        string startSign = getVn(productionTable[0]);

        // 开始符号的FOLLOW集合插入'#'符
        FOLLOW[VnToIndexMap[startSign]].insert("#");

        int cycleIndex = 1;
        do {
            isConverged = true;
            calculateFIRSTAndFOLLOWSets();

            for (auto const &vn: VnToIndexMap) {
                int index = vn.second;
                // 若非终结符的FIRST集合或FOLLOW集合发生变化，则还要进行一次循环
                if (previousFIRST[index].size() != FIRST[index].size()
                    || previousFOLLOW[index].size() != FOLLOW[index].size())
                    isConverged = false;
                previousFIRST[index] = FIRST[index];
                previousFOLLOW[index] = FOLLOW[index];
            }
        } while (!isConverged);

        //控制台输出一下结果
//    cout << endl;
        for (auto const &vn: VnToIndexMap) {
            int index = vn.second;
            if (previousFIRST[index].size() != FIRST[index].size()
                || previousFOLLOW[index].size() != FOLLOW[index].size())
                isConverged = false;

        }
    }


#endif //INTERMEDIATE_CODE_GENERATOR_GRAMMARANALYZER_H