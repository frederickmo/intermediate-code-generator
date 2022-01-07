//
// Created by Frederick Mo on 2022/1/1.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_QUADRUPLEGENERATOR_H
#define INTERMEDIATE_CODE_GENERATOR_QUADRUPLEGENERATOR_H

#include <utility>
#include <stack>

#include "SemanticAnalyzer.h"

using std::stack;
using std::string;
using std::pair;

<<<<<<< Updated upstream:QuadrupleTranslator.h
class newQuadruple;

=======
>>>>>>> Stashed changes:QuadrupleGenerator.h
class Quadruple;

class Symbol;

class Label;

class Procedure;

vector<Quadruple> newQuadrupleList;

const int offset = 100;
int nextQuad = 0 + offset;
vector<Symbol> symbolTable;
map<string, int> variableTable; // 记录哪些用户自定义符号是变量
map<string, Label> labelTable; // 记录哪些符号是label
vector<Procedure> procedureList;
map<string, int> entry;
int tempVariableCount = 0;


class Symbol {
public:
    string name;
    string val{"0"};
    string valString; // 这个属性是生成三地址代码用的
    bool isInteger{false};
    int place{-1};
    int trueExit{-1}; // 真出口
    int falseExit{-1}; // 假出口
    int nextList{-1}; // 需要在确定自己的下一条四元式的实际地址后，进行回填的四元式序列
    int quad{-1}; // 下一条语句的地址

    static Symbol generateNewTempVar();
};

class Label {
public:
    bool isDefined = false; // 是否已定义
    int quad = -1; // 跳转地址
    int nextList = -1; // 待回填地址
};

class Procedure {
public:
    string name;
    int place{};
    vector<string> parameters;
};

class Quadruple {
public:
    string op;
    bool isAssignment = true;
    bool hasArg2 = true;
    string arg1;
    bool isArg1Integer = false;
    int arg1Index = 0;
    string arg2;
    bool isArg2Integer = false;
    int arg2Index = 0;
    int resultIndex = -1;
    string resultName;

    bool isProcedureCall = false;
    string procedure;

<<<<<<< Updated upstream:QuadrupleTranslator.h
    newQuadruple(string op, string arg1, bool isArg1Integer, string resultName) :
            op(std::move(op)), arg1(std::move(arg1)), isArg1Integer(isArg1Integer),
            resultName(std::move(resultName)), hasArg2(false) {}

    newQuadruple(string op, string arg1, bool isArg1Integer,
                 string arg2, bool isArg2Integer, string resultName) :
            op(std::move(op)), arg1(std::move(arg1)), isArg1Integer(isArg1Integer),
            arg2(std::move(arg2)), isArg2Integer(isArg2Integer), resultName(std::move(resultName)) {}

    newQuadruple(string op, bool isAssignment,
                 string arg1, int arg1Index, bool isArg1Integer,
                 string arg2, int arg2Index, bool isArg2Integer,
                 string resultName, int resultIndex) :
            op(std::move(op)), isAssignment(isAssignment),
            arg1(std::move(arg1)), arg1Index(arg1Index), isArg1Integer(isArg1Integer),
            arg2(std::move(arg2)), arg2Index(arg2Index), isArg2Integer(isArg2Integer),
            resultName(std::move(resultName)), resultIndex(resultIndex) {}

    newQuadruple(bool isProcedureCall, string procedure) :
            isProcedureCall(isProcedureCall), procedure(std::move(procedure)) {}
=======
    Quadruple(string op, string arg1, bool isArg1Integer, string resultName) :
    op(std::move(op)), arg1(std::move(arg1)), isArg1Integer(isArg1Integer),
    resultName(std::move(resultName)), hasArg2(false) {}

    Quadruple(string op, string arg1, bool isArg1Integer,
              string arg2, bool isArg2Integer, string resultName) :
                 op(std::move(op)), arg1(std::move(arg1)), isArg1Integer(isArg1Integer),
                 arg2(std::move(arg2)), isArg2Integer(isArg2Integer), resultName(std::move(resultName)) {}

    Quadruple(string op, bool isAssignment,
              string arg1, int arg1Index, bool isArg1Integer,
              string arg2, int arg2Index, bool isArg2Integer,
              string resultName, int resultIndex) :
                 op(std::move(op)), isAssignment(isAssignment),
                 arg1(std::move(arg1)), arg1Index(arg1Index), isArg1Integer(isArg1Integer),
                 arg2(std::move(arg2)), arg2Index(arg2Index), isArg2Integer(isArg2Integer),
                 resultName(std::move(resultName)), resultIndex(resultIndex) {}

    Quadruple(bool isProcedureCall, string procedure) :
    isProcedureCall(isProcedureCall), procedure(std::move(procedure)) {}
>>>>>>> Stashed changes:QuadrupleGenerator.h

    void print() const {
        if (isProcedureCall)
            cout << procedure << endl;
        else if (isAssignment) {
            if (hasArg2)
                cout << resultName << " := " << arg1 << " " << op << " " << arg2 << endl;
            else if (op == "uminus")
                cout << resultName << " := uminus " << arg1 << endl;
            else
                cout << resultName << " := " << arg1 << endl;
        } else {
            cout << "(" << op << ", " << arg1Index << ", " << arg2Index << ", "
                 << (resultIndex < offset ? "uncertain" : std::to_string(resultIndex))
                 << ")" << endl;
        }
    }

    void printArgInName() const {
        if (isProcedureCall)
            cout << procedure << endl;
        else if (isAssignment) {
            if (hasArg2)
                cout << resultName << " := " << arg1 << " " << op << " " << arg2 << endl;
            else if (op == "uminus")
                cout << resultName << " := uminus " << arg1 << endl;
            else
                cout << resultName << " := " << arg1 << endl;
        } else {
            cout << "(" << op << ", " << arg1 << ", " << arg2 << ", "
                 << (resultIndex < offset ? "uncertain" : std::to_string(resultIndex))
                 << ")" << endl;
        }
    }

    void printAsQuadruple() const {
        if (isProcedureCall)
            cout << procedure << endl;
        else if (isAssignment) {
            if (hasArg2)
                cout << "(" << op << ", " << (isArg1Integer ? "#" : "") << arg1 << ", "
                     << (isArg2Integer ? "#" : "") << arg2 << ", " << resultName << ")" << endl;
            else if (op == "uminus")
                cout << "(" << "uminus" << ", " << (isArg1Integer ? "#" : "") << arg1 << ", -, " << resultName << ")"
                     << endl;
            else
                cout << "(" << op << ", " << (isArg1Integer ? "#" : "") << arg1 << ", -, " << resultName << ")" << endl;
        } else {
            cout << "(" << op << ", " << (isArg1Integer ? "#" : "") << arg1 << ", "
                 << (isArg2Integer ? "#" : "") << arg2 << ", "
                 << (resultIndex < offset ? "uncertain" : std::to_string(resultIndex))
                 << ")" << endl;

        }
    }
};

Symbol Symbol::generateNewTempVar() {
    tempVariableCount++;
    return Symbol{"T" + std::to_string(tempVariableCount)};
}

class QuadrupleGenerator {
public:

<<<<<<< Updated upstream:QuadrupleTranslator.h
    static void generateIntermediateCode(const string &op, const string &arg1, bool isArg1Integer, const string &arg2,
                                         bool isArg2Integer, int resultIndex);

    static void
    generateSingleArgThreeAddressCode(const string &result, const string &op, const string &arg, bool isArgInteger);

    static void
    generateDoubleArgThreeAddressCode(const string &result, const string &op, const string &arg1, bool isArg1Integer,
                                      const string &arg2, bool isArg2Integer);
=======
    // 产生跳转语句
    static void generateIntermediateCode(const string& op, const string& arg1, bool isArg1Integer, const string& arg2, bool isArg2Integer, int resultIndex);

    // 产生一元计算的赋值语句
    static void generateSingleArgThreeAddressCode(const string& result, const string& op, const string& arg, bool isArgInteger);

    // 产生二元计算的赋值语句
    static void generateDoubleArgThreeAddressCode(const string& result, const string& op, const string& arg1, bool isArg1Integer, const string& arg2, bool isArg2Integer);
>>>>>>> Stashed changes:QuadrupleGenerator.h

    // 产生过程调用语句
    static void generateProcedureCall(bool isProcedureCall, string procedure);


    // 合并两支待回填链表的表头
    static int merge(int listHead1, int listHead2);

    // 合并三支待回填链表的表头
    static int merge(int listHead1, int listHead2, int listHead3);

    // 回填跳转地址
    static void backPatch(int listHead, int quad);

    // 检查是否有语法错误
    static void checkError(int pos);

    // 语义分析并生成中间代码
    static void parse(bool showDetail);

    // 读取文件并生成四元式
    static void lex(bool showDetail);

    // 输出四元式
    static void printQuadruples(int choice);
};

<<<<<<< Updated upstream:QuadrupleTranslator.h
void QuadrupleTranslator::generateIntermediateCode(const string &op, const string &arg1, bool isArg1Integer,
                                                   const string &arg2, bool isArg2Integer, int resultIndex) {
//    quadrupleList.push_back(Quadruple{op, arg1, arg2, resultIndex});
//    string result = std::to_string(resultIndex);
//    quadrupleOrTAC.push_back("(" + op + ", " + arg1 + ", " + arg2 + ", " + result + ")");
    newQuadruple quadruple(op, false, arg1, entry[arg1], isArg1Integer, arg2, entry[arg2], isArg2Integer,
                           std::to_string(resultIndex), resultIndex);
=======
void QuadrupleGenerator::generateIntermediateCode(const string& op, const string& arg1, bool isArg1Integer, const string& arg2, bool isArg2Integer, int resultIndex) {
//    quadrupleList.push_back(Quadruple{op, arg1, arg2, resultIndex});
//    string result = std::to_string(resultIndex);
//    quadrupleOrTAC.push_back("(" + op + ", " + arg1 + ", " + arg2 + ", " + result + ")");
    Quadruple quadruple(op, false, arg1, entry[arg1], isArg1Integer, arg2, entry[arg2], isArg2Integer, std::to_string(resultIndex), resultIndex);
>>>>>>> Stashed changes:QuadrupleGenerator.h
    newQuadrupleList.push_back(quadruple);
    nextQuad++;
}

<<<<<<< Updated upstream:QuadrupleTranslator.h
void QuadrupleTranslator::generateDoubleArgThreeAddressCode(const string &result, const string &op, const string &arg1,
                                                            bool isArg1Integer, const string &arg2,
                                                            bool isArg2Integer) {
=======
void QuadrupleGenerator::generateDoubleArgThreeAddressCode(const string& result, const string& op, const string& arg1,
                                                            bool isArg1Integer,  const string& arg2, bool isArg2Integer) {
>>>>>>> Stashed changes:QuadrupleGenerator.h
//    quadrupleOrTAC.push_back(result + " := " + expression);
    Quadruple quadruple(op, arg1, isArg1Integer, arg2, isArg2Integer, result);
    newQuadrupleList.push_back(quadruple);
    ++nextQuad;
}

<<<<<<< Updated upstream:QuadrupleTranslator.h
void QuadrupleTranslator::generateSingleArgThreeAddressCode(const string &result, const string &op, const string &arg,
                                                            bool isArgInteger) {
    newQuadruple quadruple(op, arg, isArgInteger, result);
=======
void QuadrupleGenerator::generateSingleArgThreeAddressCode(const string &result, const string &op, const string &arg, bool isArgInteger) {
    Quadruple quadruple(op, arg, isArgInteger, result);
>>>>>>> Stashed changes:QuadrupleGenerator.h
    newQuadrupleList.push_back(quadruple);
    ++nextQuad;
}

void QuadrupleGenerator::generateProcedureCall(bool isProcedureCall, string procedure) {
    Quadruple quadruple(isProcedureCall, std::move(procedure));
    newQuadrupleList.push_back(quadruple);
    ++nextQuad;
}


// 合并两个需要回填的链表的表头
int QuadrupleGenerator::merge(int listHead1, int listHead2) {
    // 如果其中一个为0（就是没有需要回填的地址）另一个不为0，就返回不为0那个
    if (listHead1 == -1 || listHead2 == -1) {
        if (listHead1 == -1 && listHead2 != -1)
            return listHead2;
        else if (listHead1 != -1)
            return listHead1;
    } else {
        // 递归寻找表头：每个跳转四元式的第四位在填写真正的跳转地址前，
        // 填写的是等待被回填的所有四元式组成的链表中的上一个四元式的序号。
        // 该链表的表头因为没有上一个四元式了，所以第四位填写的是第一个地址即offset，
        // 因此一直找到最后一位为offset的四元式，就是该链表的表头。
        int nextQuadIndex = listHead2;
        while (newQuadrupleList.at(nextQuadIndex).resultIndex >= 0) {
            nextQuadIndex = newQuadrupleList.at(nextQuadIndex).resultIndex;
        }
        // 此时tempHead指向的已经是表头，把表头指向的地址指向list1的链尾，完成合并操作。
        newQuadrupleList.at(nextQuadIndex).resultIndex = listHead1;
        return listHead2;
    }
}

int QuadrupleGenerator::merge(int listHead1, int listHead2, int listHead3) {
    // 我想复杂了，调用二参数的merge递归一下就行了
    return merge(listHead1, merge(listHead2, listHead3));
}

/**
 * CORE:【关于回填】
 * 正式生成的四元式里每一条的序号(即地址)
 * 都是其在四元式列表里的索引加上了一个偏移量(offset=100)得来。
 * 对于跳转语句，为了区分已经有确定跳转地址的语句和没有确定跳转地址(等待回填)的语句，
 * 在填写假地址等待回填的时候，填的是没有加上偏移量的四元式索引。
 * 因此可以通过跳转语句的跳转地址是否大于等于偏移量来得知是否已经回填。
 */

// 回填，在得到一支跳转地址相同但未确定的四元式的确定跳转地址后，用该地址回填这些四元式
void QuadrupleGenerator::backPatch(int listHead, int quad) {
    if (listHead < 0 || quad < offset)
        return;

    int tmpIndex = listHead;

    do {
        // 目前正在回填的四元式的第四位即下一个需要回填的四元式的序号
        int nextQuadrupleIndex = newQuadrupleList.at(tmpIndex).resultIndex;
        // 回填操作
        newQuadrupleList.at(tmpIndex).resultIndex = quad;
        // 更新寻找下一条
        tmpIndex = nextQuadrupleIndex;
    } while (tmpIndex >= 0);

}

void QuadrupleGenerator::checkError(int pos) {
    std::cerr << "存在语法错误\n";
    for (int lineCount = 0; lineCount < locateInputCode.size(); ++lineCount) {
        if (locateInputCode[lineCount] > pos) {
            cout << lineCount << endl;
            break;
        }
    }
    exit(0);
}

static void printStateStack(const stack<int> &stateStack) {
    cout << "状态栈 ";
    stack<int> copy = stateStack;
    stack<int> reversed;
    while (!copy.empty()) {
        reversed.push(copy.top());
        copy.pop();
    }
    while (!reversed.empty()) {
        cout << reversed.top() << " ";
        reversed.pop();
    }
    cout << endl;
}

static void printSymbolStack(const stack<Symbol> &symbolStack) {
    cout << "符号栈 ";
    stack<Symbol> copy = symbolStack;
    stack<Symbol> reversed;
    while (!copy.empty()) {
        reversed.push(copy.top());
        copy.pop();
    }
    while (!reversed.empty()) {
        cout << reversed.top().name << " ";
        reversed.pop();
    }
    cout << endl;
}

void QuadrupleGenerator::parse(bool showDetail) {
    // 初始化状态栈/符号栈/语义栈
    stack<int> stateStack; // 状态栈
    stack<Symbol> symbolStack; // 符号栈
    stack<Symbol> tempVarStack; // 临时变量栈
    stack<string> semanticStack; // 语义栈
    int inputPointer = 0; // 输入串指针
    stateStack.push(0); // 状态栈初始化：填入0装#
    symbolStack.push(Symbol{"#"}); // 符号栈初始化：填入0

//    generateIntermediateCode("j", "-", "-", offset);
    int oldPointer = -1; // 存入上一次操作的输入串指针

    while (!symbolStack.empty()) {
        // 取状态栈栈顶的状态进行判断
        int curState = stateStack.top();
        // 读取输入串的下一个字符
        string symbolToRead; // 为了能读空字，所以这里先声明

        string nextSymbol = lexicalTable[inputPointer].token;
        cout << "";

<<<<<<< Updated upstream:QuadrupleTranslator.h
        // FIXME: 读空字存在问题：句末为空的时候如果ACTION表里能读空字会去读空字，目前的临时办法是句末带#符号，但这肯定不对，需要解决
        // FIXME:  是不是应该只有shift才读空字？
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> Stashed changes:QuadrupleGenerator.h
        if (ActionTable[curState][VtToIndexMap["null"]] != ERROR &&
            !(lexicalTable[inputPointer].token == "#")) {
=======
        if (ActionTable[curState][VtToIndex["null"]] != ERROR &&
        !(lexicalTable[inputPointer].token == "#")) {
>>>>>>> parent of 30ad1b1 (更新)
=======
        if (ActionTable[curState][VtToIndex["null"]] != ERROR &&
        !(lexicalTable[inputPointer].token == "#")) {
>>>>>>> parent of 30ad1b1 (更新)
            // 如果该状态一行可以读空字且下一个符号不是结束符#，则读入一个空字
            symbolToRead = "null";
        } else {
            symbolToRead = lexicalTable[inputPointer].token;
            if (lexicalTable[inputPointer].indexInKeywords == VARIABLE
                || lexicalTable[inputPointer].indexInKeywords == INTEGER) {
                // 在符号表中记作i
                // => 完成未写出的产生式
                symbolToRead = "i";
                if (oldPointer != inputPointer) {
                    // 如果输入串指针没有变化，则不读新符号进栈

                    // 语义栈入栈
                    semanticStack.push(lexicalTable[inputPointer].token);

                    // 对常数数字和变量的共同处理：name和val统一
                    Symbol tmpSymbol;
                    if (entry.count(lexicalTable[inputPointer].token) == 0) {
                        // 新符号
                        // => 加入符号表，添加入口地址映射
                        tmpSymbol.name = lexicalTable[inputPointer].token;
                        tmpSymbol.val = lexicalTable[inputPointer].token;
                        tmpSymbol.valString = tmpSymbol.val;
                        tmpSymbol.isInteger = lexicalTable[inputPointer].indexInKeywords == INTEGER;
                        tmpSymbol.place = symbolTable.size();
                        symbolTable.push_back(tmpSymbol);
                        entry[tmpSymbol.name] = tmpSymbol.place;
                    }
                    // 符号表其中已经有的符号不进行上述处理
                }
                // 更新指针
                oldPointer = inputPointer;
            }
        }

        // 变量或数字一起当做终结符i处理
        // 当前状态面临栈外输入串头部符号，ACTION表内的状态
        int curSymbolIndex = ActionTable[curState][VtToIndex[symbolToRead]];

        // 如果当前符号表位置为0(ERROR)，报错
        if (curSymbolIndex == ERROR) {
            if (showDetail)
                cout << "【错误】在状态 " << curState << " 发生错误\n";
            checkError(inputPointer);
            exit(0);
        } else if (isShiftTerm(curSymbolIndex)) {
            if (showDetail)
                cout << "【移进】从状态 " << curState << " 移进状态 " << getShiftStateIndex(curSymbolIndex) << endl;
            // 移进项目 => 直接入栈，转向新状态
            stateStack.push(getShiftStateIndex(curSymbolIndex));
            symbolStack.push(Symbol{symbolToRead});
            // 栈外指针指向下一位
            if (symbolToRead != "null")
                inputPointer++;
            if (showDetail) {
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            }
        } else if (isReduceTerm(curSymbolIndex)) {
            if (showDetail)
                cout << "【规约】在状态 " << curState << " 使用第 " << getReduceStateIndex(curSymbolIndex) << " 个产生式进行规约\n";
            // 规约项目 => 找到是用哪个串进行规约的
            int whichProductionToReduce = getReduceStateIndex(curSymbolIndex);
            // 被规约的串
            Term reduceTerm;
            // 当前产生式
            string production = productionTable[whichProductionToReduce];
            GrammarAnalyzer::split(production, reduceTerm.leftPart, reduceTerm.rightPart);

            if (production == "<<FACTOR>>-><<ID>>"
                || production == "<<TERM>>-><<FACTOR>>"
                || production == "<<NEG>>-><<TERM>>"
                || production == "<<EXPR>>-><<NEG>>") {
                // 取栈顶串进行归约
                // 产生式13,15,23,43的归约

                Symbol topSymbol = symbolStack.top();

                if (production == "<<FACTOR>>-><<ID>>") {
//                    cout << "当前被归约的ID的valstring:" << topSymbol.valString << endl;
                    if (labelTable.count(topSymbol.valString) != 0) {
                        std::cerr << "变量 " << topSymbol.valString
                                  << " 已被定义为label标识符\n";
                        checkError(0);
                    } else {
                        variableTable[topSymbol.valString]++;
                    }
                }

                symbolStack.pop();
                stateStack.pop();
                // 规约的产生式左部进栈
                symbolStack.push(Symbol{reduceTerm.leftPart});
                // 根据现在栈顶状态计算状态转移
                curState = stateStack.top();
                // 新状态进栈
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);
                symbolStack.top().place = topSymbol.place;
                symbolStack.top().valString = topSymbol.valString;
                symbolStack.top().isInteger = topSymbol.isInteger;

            } else if (production == "<<ID>>->i") {
                // 产生式17的归约

                symbolStack.pop();
                stateStack.pop();
                // 将该被归约的非终结符进栈
                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                // 新状态进栈
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);
                // 记录为该符号的入口地址

                // valString记录的是终结符i的名字，名字为了在调试过程中能看懂归约到哪一步了所以保持<<ID>>不变
                symbolStack.top().place = entry[semanticStack.top()];
                symbolStack.top().valString = semanticStack.top();
                bool isIncluded = false;
                for (const auto &symbol : symbolTable) {
                    if (symbol.name == symbolStack.top().name) {
                        symbolStack.top().isInteger = symbol.isInteger;
                        isIncluded = true;
                        break;
                    }
                }
                if (!isIncluded)
                    symbolStack.top().isInteger = symbolTable.back().isInteger;

            } else if (production == "<<RELOP>>->="
<<<<<<< Updated upstream:QuadrupleTranslator.h
                       || production == "<<RELOP>>->!="
                       || production == "<<RELOP>>-><"
                       || production == "<<RELOP>>-><="
                       || production == "<<RELOP>>->>"
                       || production == "<<RELOP>>->>=") {
                /**
                 * CORE:{27,28,29,30,31,32}【语义处理】读取关系运算符
                 */
=======
            || production == "<<RELOP>>->!="
            || production == "<<RELOP>>-><"
            || production == "<<RELOP>>-><="
            || production == "<<RELOP>>->>"
            || production == "<<RELOP>>->>=") {
                 // 产生式27,28,29,30,31,32的归约
>>>>>>> Stashed changes:QuadrupleGenerator.h

                symbolStack.pop();
                stateStack.pop();
                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().val = semanticStack.top();
                symbolStack.top().valString = reduceTerm.rightPart.at(0);

            } else if (production == "<<BCMP>>->(<<BEXPR>>)") {
                // 产生式6的归约
                Symbol booleanExpr;
                for (int count = 0; count < 3; ++count) {
                    if (count == 1)
                        booleanExpr = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().trueExit = booleanExpr.trueExit;
                symbolStack.top().falseExit = booleanExpr.falseExit;

            } else if (production == "<<BCMP>>-><<EXPR>>") {
                // 产生式5的归约
                Symbol booleanExpr = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().trueExit = nextQuad - offset;
                symbolStack.top().falseExit = nextQuad + 1 - offset;
                generateIntermediateCode("jnz", booleanExpr.valString, booleanExpr.isInteger, "-", false, -1);
                generateIntermediateCode("j", "-", false, "-", false, -1);

            } else if (production == "<<BCMP>>-><<EXPR>><<RELOP>><<EXPR>>") {
                // 产生式4的归约

                 Symbol expression1, expression2, relop;
                 // 符号栈弹出三位，其中top是第二个操作符，top-2是第一个操作符
                 for (int count = 0; count < 3; ++count) {
                     if (count == 0)
                         expression2 = symbolStack.top();
                     else if (count == 2)
                         expression1 = symbolStack.top();
                     else
                         relop = symbolStack.top();
                     symbolStack.pop();
                     stateStack.pop();
                 }

                 symbolStack.push(Symbol{reduceTerm.leftPart});
                 curState = stateStack.top();
                 stateStack.push(
                         GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                 // 给出真假出口地址
                 symbolStack.top().trueExit = nextQuad - offset;
                 symbolStack.top().falseExit = nextQuad + 1 - offset;

                 // 生成两条语句
                string place1Str = std::to_string(expression1.place);
                string place2str = std::to_string(expression2.place);
                generateIntermediateCode("j" + relop.valString, expression1.valString,
<<<<<<< Updated upstream:QuadrupleTranslator.h
                                         expression1.isInteger, expression2.valString, expression2.isInteger, 0);
                generateIntermediateCode("j", "-", false, "-", false, 0);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
=======
                                         expression1.isInteger, expression2.valString, expression2.isInteger, -1);
                generateIntermediateCode("j", "-", false, "-", false,-1);

>>>>>>> Stashed changes:QuadrupleGenerator.h
            } else if (production == "A-><<ID>>:=<<EXPR>>") {
                // 产生式1的归约

                Symbol id, expression;

                // 弹出三个符号: id, :=(赋值符号), expr
                for (int count = 0; count < 3; ++count) {
                    if (count == 0)
                        expression = symbolStack.top();
                    else if (count == 2)
                        id = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                if (labelTable.count(id.valString) != 0) {
                    std::cerr << "变量 " << id.valString << " 已被定义为label标识符\n";
                    checkError(0);
                } else {
                    // 未被定义为标识符，则定义为变量
                    variableTable[id.valString]++;
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);


                semanticStack.pop();
                if (!tempVarStack.empty()) {
                    tempVarStack.pop();
                    semanticStack.pop();
                }
                id.name = semanticStack.top();
                semanticStack.pop();
                // 更新符号的入口地址
                id.place = entry[id.name];
                // 值更新为表达式的值(的入口地址)
                id.val = symbolTable[expression.place].val;
                // 同时更新symbolTable
                symbolTable[id.place].val = symbolTable[expression.place].val;

                // 产生一句赋值语句

                string placeStr = std::to_string(expression.place);
                generateSingleArgThreeAddressCode(id.valString, ":=", expression.valString, expression.isInteger);

            } else if (production == "S->A"
<<<<<<< Updated upstream:QuadrupleTranslator.h
                       || production == "L->S"
                       || production == "<<STMT>>->S"
                       || production == "<<START>>-><<STMT>>"
                       || production == "<<STMT>>-><<OPENSTMT>>") {
                /**
                 * S->A => 从assignment(赋值语句)归约到statement(一般语句)
                 * <<STMT>>->S => 从一般语句规约到一般的语句(stmt还包含open_stmt)
                 * <<START>>-><<STMT>> => 反正开始也是一个语句，感觉也可以这样归约
                 *
                 * => 按照原版的 statement -> { statement }这样规约的。
                 */

                // CORE:{34,40,41}     【语义处理】从赋值语句(assignment)归约到一般语句(statement)再归约到开始符号
=======
            || production == "L->S"
            || production == "<<STMT>>->S"
            || production == "<<START>>-><<STMT>>"
            || production == "<<STMT>>-><<OPENSTMT>>") {
                // 产生式34,40,41的归约

>>>>>>> Stashed changes:QuadrupleGenerator.h
                // 顶部语句
                Symbol statement = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().place = statement.place;
                symbolStack.top().val = statement.val;
                symbolStack.top().valString = statement.valString;
                symbolStack.top().trueExit = statement.trueExit;
                symbolStack.top().falseExit = statement.falseExit;

                if (production == "L->S")
                    symbolStack.top().nextList = statement.nextList;

            } else if (production == "<<EXPR>>-><<EXPR>>+<<TERM>>"
<<<<<<< Updated upstream:QuadrupleTranslator.h
                       || production == "<<TERM>>-><<TERM>>*<<FACTOR>>") {
                // CORE:{14,44}     【语义处理】算术运算：从'+'运算归约到表达式(expression) or 从'*"运算归约到表达式
=======
            || production == "<<TERM>>-><<TERM>>*<<FACTOR>>") {
                // 产生式14，44的归约
>>>>>>> Stashed changes:QuadrupleGenerator.h

                Symbol expression1, expression2;
                for (int count = 0; count < 3; ++count) {
                    if (count == 0)
                        expression2 = symbolStack.top();
                    else if (count == 2)
                        expression1 = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }


                Symbol tempVar = Symbol::generateNewTempVar();
                // 临时变量地址放在符号表末尾(索引=符号表长度)
                tempVar.place = symbolTable.size();
                tempVar.valString = expression1.valString + reduceTerm.rightPart[1] + expression2.valString;

                // 临时变量存入符号表，entry表记录临时变量地址
                symbolTable.push_back(tempVar);
                entry[tempVar.name] = tempVar.place;
                // 临时变量存入临时变量表 => 可能会产生冗余的临时变量，不管了。。
                tempVarStack.push(tempVar);

                Symbol newLeft = Symbol{reduceTerm.leftPart};
                newLeft.valString = tempVar.name;
                symbolStack.push(newLeft);
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);
<<<<<<< HEAD
=======

>>>>>>> parent of 30ad1b1 (更新)


                generateDoubleArgThreeAddressCode(tempVar.name, reduceTerm.rightPart[1], expression1.valString,
                                                  expression1.isInteger, expression2.valString, expression2.isInteger);

            } else if (production == "<<NEG>>->-<<TERM>>") {
                // 产生式24的归约

                Symbol expression = symbolStack.top();
                symbolStack.pop();
                symbolStack.pop();
                stateStack.pop();
                stateStack.pop();

                Symbol tempVar = Symbol::generateNewTempVar();
                tempVar.place = symbolTable.size();
                tempVar.valString = "uminus " + expression.valString;

                symbolTable.push_back(tempVar);
                entry[tempVar.name] = tempVar.place;

                tempVarStack.push(tempVar);

                Symbol newLeft = Symbol{reduceTerm.leftPart};
                newLeft.valString = tempVar.name;
                symbolStack.push(newLeft);
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                generateSingleArgThreeAddressCode(tempVar.name, "uminus", expression.valString, expression.isInteger);

            } else if (production == "<<FACTOR>>->(<<EXPR>>)") {
                // 产生式16的归约

                // 加上括号一共弹出三个符号
                Symbol expression;
                for (int count = 0; count < 3; ++count) {
                    if (count == 1)
                        expression = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();

                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().place = expression.place;
                symbolStack.top().valString = expression.valString;

            } else if (production == "<<BEXPR>>-><<BAND>>"
<<<<<<< Updated upstream:QuadrupleTranslator.h
                       || production == "<<BAND>>-><<BNOT>>"
                       || production == "<<BNOT>>-><<BCMP>>") {
                // CORE: {2,7,9}【语义分析】布尔表达式递归归约
=======
            || production == "<<BAND>>-><<BNOT>>"
            || production == "<<BNOT>>-><<BCMP>>") {
                // 产生式2,7,9的归约
>>>>>>> Stashed changes:QuadrupleGenerator.h

                // 记录产生式右部符号
                Symbol rightSymbol = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 布尔表达式，传递真假出口
                symbolStack.top().trueExit = rightSymbol.trueExit;
                symbolStack.top().falseExit = rightSymbol.falseExit;
                // 下面这句不知道啥意思，但加上了
                symbolStack.top().valString = rightSymbol.valString;

            } else if (production == "<<OPENSTMT>>->if<<BEXPR>>thenM<<STMT>>") {
                // 产生式25的归约

                // 符号栈和状态栈弹出五次，记录BEXPR, M和STMT
                Symbol booleanExpression, M, statement;
                for (int count = 0; count < 5; ++count) {
                    if (count == 0)
                        statement = symbolStack.top();
                    else if (count == 1)
                        M = symbolStack.top();
                    else if (count == 3)
                        booleanExpression = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 回填和合并链表
                backPatch(booleanExpression.trueExit, M.quad);
                symbolStack.top().nextList = merge(booleanExpression.falseExit, statement.nextList);

            } else if (production == "S->if<<BEXPR>>thenMSNelseMS"
<<<<<<< Updated upstream:QuadrupleTranslator.h
                       || production == "<<OPENSTMT>>->if<<BEXPR>>thenMSNelseM<<OPENSTMT>>") {
                // CORE:{26,33}【语义分析】if then else语句的翻译
=======
            || production == "<<OPENSTMT>>->if<<BEXPR>>thenMSNelseM<<OPENSTMT>>") {
                // 产生式26，33的归约
>>>>>>> Stashed changes:QuadrupleGenerator.h

                // 一共9个符号：一共需要弹出9次
                // if(8) <<BEXPR>>(7) then(6) M(5) S(4) N(3) else(2) M(1) S(0)
                Symbol booleanExpr, M1, statement1, N, M2, statement2;
                for (int count = 0; count < 9; ++count) {
                    if (count == 0)
                        statement2 = symbolStack.top();
                    else if (count == 1)
                        M2 = symbolStack.top();
                    else if (count == 3)
                        N = symbolStack.top();
                    else if (count == 4)
                        statement1 = symbolStack.top();
                    else if (count == 5)
                        M1 = symbolStack.top();
                    else if (count == 7)
                        booleanExpr = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                backPatch(booleanExpr.trueExit, M1.quad);
                backPatch(booleanExpr.falseExit, M2.quad);
                symbolStack.top().nextList = merge(statement1.nextList, N.nextList, statement2.nextList);

            } else if (production == "M->null"
<<<<<<< Updated upstream:QuadrupleTranslator.h
                       || production == "N->null") {
                //CORE{21,22}【语义分析】规约为空字
=======
            || production == "N->null") {
                // 产生式21,22的归约
>>>>>>> Stashed changes:QuadrupleGenerator.h

                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 接下来是对于M和N的两种不同处理
                // quad记录的是经偏移后的真实地址，
                // truelist/falselist/nextlist记录的是减去偏移的虚拟地址，
                // 虚拟地址小于偏移量100的意思就是还没有确定真实的地址。
                if (reduceTerm.leftPart == "M") {
                    // M是记录nextquad
                    symbolStack.top().quad = nextQuad;
                } else if (reduceTerm.leftPart == "N") {
                    // N的nextlist记录nextquad，意思是N产生的四元式的跳转地址等待回填
                    symbolStack.top().nextList = nextQuad - offset;
                    generateIntermediateCode("j", "-", false, "-", false, -1);
                }

            } else if (production == "L->L;MS") {
                // 产生式18的归约

                Symbol L, M, statement;
                for (int count = 0; count < 4; ++count) {
                    if (count == 0)
                        symbolStack.top() = statement;
                    else if (count == 1)
                        symbolStack.top() = M;
                    else if (count == 3)
                        symbolStack.top() = L;
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                backPatch(L.nextList, M.quad);
                L.nextList = statement.nextList;

            } else if (production == "S->{L}") {
                // 产生式35的归约

                Symbol L;
                for (int count = 0; count < 3; ++count) {
                    if (count == 1)
                        L = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().nextList = L.nextList;

            } else if (production == "<<LABEL>>-><<ID>>:") {
                // 产生式20的归约

                symbolStack.pop();
                stateStack.pop();
                Symbol label = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();


                if (variableTable[label.valString] != 0) {
                    std::cerr << "label标识符 "
                              << label.valString << " 已被定义为变量\n";
                    checkError(0);
                }

                // label分三种情况：
                // ① label已定义(已有跳转地址，重新赋跳转地址)则报错
                // ② label已在符号表中，但是没有地址，赋地址
                //      (这是先有goto label再label: ...的情况)
                // ③ label不在符号表中，加入符号表
                //      (这是现有label: ...的情况)
                if (labelTable.count(label.valString) != 0) {
                    if (labelTable[label.valString].quad != -1) {
                        std::cerr << "label标识符 " << label.valString << " 被重复定义\n";
                        checkError(0);
                    } else {
                        // 这里是已经有goto该label但是label本身还没有定义
                        labelTable[label.valString].quad = nextQuad;
                        backPatch(labelTable[label.valString].nextList, nextQuad);
                    }
                } else {
                    // 还没有别的四元式goto到该label，所以先赋一个地址
                    labelTable[label.valString].quad = nextQuad;
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

            } else if (production == "S-><<LABEL>>S") {
                // 产生式38的归约

                Symbol statement = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

            } else if (production == "S->goto<<ID>>") {
                // 产生式39的归约

                Symbol label = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);
<<<<<<< HEAD
=======


>>>>>>> parent of 30ad1b1 (更新)


                int address = -1;
                if (labelTable.count(label.valString) == 0) {
                    labelTable[label.valString].nextList =
                            merge(labelTable[label.valString].nextList,
                                  nextQuad - offset);
                } else {
                    if (labelTable[label.valString].quad != -1) {
                        address = labelTable[label.valString].quad;
                    }
                        // 否则将该地址添加到label的nextList里
                    else
<<<<<<< Updated upstream:QuadrupleTranslator.h
//                        labelTable[label.valString].nextList =
//                                merge(labelTable[label.valString].nextList,
//                                      nextQuad - offset);
                        labelTable[label.valString].nextList =
                                merge(nextQuad - offset,
                                      labelTable[label.valString].nextList);
=======
                    labelTable[label.valString].nextList =
                            merge(nextQuad - offset,
                                  labelTable[label.valString].nextList);
>>>>>>> Stashed changes:QuadrupleGenerator.h
                }

                generateIntermediateCode("j", "-", false, "-", false, address);

<<<<<<< Updated upstream:QuadrupleTranslator.h

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "S->whileM<<BEXPR>>doMS") {
                //CORE:{36}【语义分析】while语句 S->whileM<<BEXPR>>doMS
=======
            }
            else if (production == "S->whileM<<BEXPR>>doMS") {
                // 产生式36的归约
>>>>>>> Stashed changes:QuadrupleGenerator.h

                // 一共6个符号：一共需要弹出6次
                // while(5) M(4) <<BEXPR>>(3) do(2) M(1) S(0)
                Symbol M1, booleanExpr, M2, statement;
                for (int count = 0; count < 6; ++count) {
                    if (count == 0)
                        statement = symbolStack.top();
                    else if (count == 1)
                        M2 = symbolStack.top();
                    else if (count == 3)
                        booleanExpr = symbolStack.top();
                    else if (count == 4)
                        M1 = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                //回填
                backPatch(statement.nextList, M1.quad);
                backPatch(booleanExpr.trueExit, M2.quad);
                symbolStack.top().nextList = booleanExpr.falseExit;

                generateIntermediateCode("j", "-", false, "-", false, M1.quad);

            } else if (production == "S->call<<ID>>(<<ELIST>>)") {
                // 产生式37的归约

                Symbol id;
                for (int count = 0; count < 5; ++count) {
                    if (count == 3)
                        id = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                Procedure function = procedureList.back();
                function.name = id.valString;
                for (const auto &param : function.parameters)
                    generateProcedureCall(true, "param " + param);
                generateProcedureCall(true, "call " + function.name);

            } else if (production == "<<ELIST>>-><<EXPR>>") {
                // 产生式11的归约

                Symbol expr = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                Procedure procedure;
                procedure.parameters.push_back(expr.valString);
                procedureList.push_back(procedure);

            } else if (production == "<<ELIST>>-><<ELIST>>,<<EXPR>>") {
                // 产生式12的归约

                Symbol expr = symbolStack.top();
                for (int count = 0; count < 3; ++count) {
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                procedureList.back().parameters.push_back(expr.valString);

            } else if (production == "<<BNOT>>->not<<BNOT>>") {
                // 产生式10的归约
                Symbol current = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();
                symbolStack.pop();
                stateStack.pop();


                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                symbolStack.top().trueExit = current.falseExit;
                symbolStack.top().falseExit = current.trueExit;

            } else if (production == "<<BAND>>-><<BAND>>andM<<BNOT>>") {
                // 产生式3的归约
                Symbol bool1, bool2, M;

                bool2 = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                M = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                //and
                symbolStack.pop();
                stateStack.pop();

                bool1 = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();


                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                backPatch(bool1.trueExit, M.quad);
                symbolStack.top().trueExit = bool2.trueExit;
                symbolStack.top().falseExit = merge(bool1.falseExit, bool2.falseExit);

            } else if (production == "<<BEXPR>>-><<BEXPR>>orM<<BAND>>") {
                // 产生式8的归约

                Symbol bool1, bool2, M;

                bool2 = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();
                M = symbolStack.top();

                symbolStack.pop();
                stateStack.pop();

                //or
                symbolStack.pop();
                stateStack.pop();

                bool1 = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();


                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                backPatch(bool1.falseExit, M.quad);
                symbolStack.top().trueExit = merge(bool1.trueExit, bool2.trueExit);
                symbolStack.top().falseExit = bool2.falseExit;

            }

            if (showDetail) {
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            }
        } else if (isAcceptTerm(curSymbolIndex)) {
            // 接受状态
            cout << endl << "ACCEPT" << endl << endl;
            // 弹出最后一个状态和预先放入状态栈的0
            stateStack.pop();
            symbolStack.pop();
            stateStack.pop();
            symbolStack.pop();
        }
    }
}

void QuadrupleGenerator::lex(bool showDetail) {
    string inputCode;
    std::ifstream inFile("input.txt");
    cout << "------------------中间代码生成----------------------\n";
    locateInputCode.push_back(0);
    while (std::getline(inFile, inputCode)) {
        cout << "输入串:" << endl;
        cout << inputCode << endl;
        inputCode.push_back('#');
        Tokenizer::lexicalAnalyze(inputCode);
        locateInputCode.push_back(lexicalTableLength);
    }

<<<<<<< Updated upstream:QuadrupleTranslator.h
    cout << "\nlexical table: \n";
    for (const auto &word : lexicalTable)
        word.print();
=======
    if (showDetail) {
        cout << "\nlexical table: \n";
        for (const auto& word : lexicalTable)
            word.print();
    }

}

void QuadrupleGenerator::printQuadruples(int choice = 0) {
    int address = offset;
    switch(choice) {
        default:
        case 0:
            cout << "四元式形式\n";
            for (const auto& quadruple : newQuadrupleList) {
                cout << "(" << address << ") ";
                quadruple.printAsQuadruple();
                ++address;
            }
            cout << endl;
            break;
        case 1:
            cout << "三地址代码形式\n";
            for (const auto& quadruple : newQuadrupleList) {
                cout << "(" << address << ") ";
                quadruple.printArgInName();
                ++address;
            }
        case 2:
            cout << "四元式形式(变量以地址形式输出)\n";
            for (const auto& quadruple : newQuadrupleList) {
                cout << "(" << address << ") ";
                quadruple.print();
                ++address;
            }
>>>>>>> Stashed changes:QuadrupleGenerator.h


    }
}


#endif //INTERMEDIATE_CODE_GENERATOR_QUADRUPLEGENERATOR_H
