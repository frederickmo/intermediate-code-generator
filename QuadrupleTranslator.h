//
// Created by Frederick Mo on 2022/1/1.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_QUADRUPLETRANSLATOR_H
#define INTERMEDIATE_CODE_GENERATOR_QUADRUPLETRANSLATOR_H

#include <utility>
#include <stack>

#include "SemanticAnalyzer.h"

using std::stack;
using std::string;
using std::pair;

class newQuadruple;
class Quadruple;
class Symbol;
class Label;
class Procedure;

vector<newQuadruple> newQuadrupleList;

vector<Quadruple> quadrupleList;
vector<string> quadrupleOrTAC; // quadruple(四元式)或者TAC(Three Address Code, 三地址代码)
const int offset = 100;
int nextQuad = 0 + offset;
vector<int> addresses; // 四元式地址表
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
    int place;
    vector<string> parameters;
};

struct Quadruple {
    string op;
    std::basic_string<char, std::char_traits<char>, std::allocator<char>> arg1Index;
    std::basic_string<char, std::char_traits<char>, std::allocator<char>> arg2Index;
    int resultIndex;

    void print() const {
        cout << "(" << op << ", " << arg1Index << ", " << arg2Index << ", " << resultIndex << ")\n";
    }
};

class ComplexQuadruple {
public:
    Symbol argList[4];
    int index{};
};

class newQuadruple {
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
    int resultIndex = 0;
    string resultName;

    bool isProcedureCall = false;
    string procedure;

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
                cout << "(" << "uminus" << ", " << (isArg1Integer ? "#" : "") << arg1 << ", -, " << resultName << ")" << endl;
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
    return Symbol{"T"+std::to_string(tempVariableCount)};
}

class QuadrupleTranslator {
public:

    static void generateIntermediateCode(const string& op, const string& arg1, bool isArg1Integer, const string& arg2, bool isArg2Integer, int resultIndex);

    static void generateSingleArgThreeAddressCode(const string& result, const string& op, const string& arg, bool isArgInteger);

    static void generateDoubleArgThreeAddressCode(const string& result, const string& op, const string& arg1, bool isArg1Integer, const string& arg2, bool isArg2Integer);

    static void generateProcedureCall(bool isProcedureCall, string procedure);


    static int merge(int listHead1, int listHead2);

    static int merge(int listHead1, int listHead2, int listHead3);

    static void backPatch(int listHead, int quad);

    static void checkError(int pos);

    // 语义分析并生成中间代码
    static void parse();

    // 读取文件并生成四元式
    static void lex();
};

void QuadrupleTranslator::generateIntermediateCode(const string& op, const string& arg1, bool isArg1Integer, const string& arg2, bool isArg2Integer, int resultIndex) {
//    quadrupleList.push_back(Quadruple{op, arg1, arg2, resultIndex});
//    string result = std::to_string(resultIndex);
//    quadrupleOrTAC.push_back("(" + op + ", " + arg1 + ", " + arg2 + ", " + result + ")");
    newQuadruple quadruple(op, false, arg1, entry[arg1], isArg1Integer, arg2, entry[arg2], isArg2Integer, std::to_string(resultIndex), resultIndex);
    newQuadrupleList.push_back(quadruple);
    nextQuad++;
}

void QuadrupleTranslator::generateDoubleArgThreeAddressCode(const string& result, const string& op, const string& arg1,
                                                            bool isArg1Integer,  const string& arg2, bool isArg2Integer) {
//    quadrupleOrTAC.push_back(result + " := " + expression);
    newQuadruple quadruple(op, arg1, isArg1Integer, arg2, isArg2Integer, result);
    newQuadrupleList.push_back(quadruple);
    ++nextQuad;
}

void QuadrupleTranslator::generateSingleArgThreeAddressCode(const string &result, const string &op, const string &arg, bool isArgInteger) {
    newQuadruple quadruple(op, arg, isArgInteger, result);
    newQuadrupleList.push_back(quadruple);
    ++nextQuad;
}

void QuadrupleTranslator::generateProcedureCall(bool isProcedureCall, string procedure) {
    newQuadruple quadruple(isProcedureCall, std::move(procedure));
    newQuadrupleList.push_back(quadruple);
    ++nextQuad;
}


//FIXME:这个待测试
int QuadrupleTranslator::merge(int listHead1, int listHead2) {
    // 如果其中一个为0（就是没有需要回填的地址）另一个不为0，就返回不为0那个
    if (listHead1 == -1 || listHead2 == -1) {
        if (listHead1 == -1 && listHead2 != -1)
            return listHead2;
        else if (listHead1 != -1)
            return listHead1;
    }
    else {
        // 递归寻找表头：每个跳转四元式的第四位在填写真正的跳转地址前，
        // 填写的是等待被回填的所有四元式组成的链表中的上一个四元式的序号。
        // 该链表的表头因为没有上一个四元式了，所以第四位填写的是第一个地址即offset，
        // 因此一直找到最后一位为offset的四元式，就是该链表的表头。
        int nextQuadIndex = listHead2;
        while (newQuadrupleList.at(nextQuadIndex).resultIndex > 0) {
            nextQuadIndex = newQuadrupleList.at(nextQuadIndex).resultIndex;
        }
        // 此时tempHead指向的已经是表头，把表头指向的地址指向list1的链尾，完成合并操作。
        newQuadrupleList.at(nextQuadIndex).resultIndex = listHead1;
        return listHead2;
    }
}

int QuadrupleTranslator::merge(int listHead1, int listHead2, int listHead3) {
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
void QuadrupleTranslator::backPatch(int listHead, int quad) {
    if (listHead == -1 || quad == 0)
        return;

    int tmpIndex = listHead;

    // ATTENTION: 对于链首也需要回填的地方，while会跳过第一个需要回填的式子，改成了do while多一次循环试试看
    do {
        // 目前正在回填的四元式的第四位即下一个需要回填的四元式的序号
        int nextQuadrupleIndex = newQuadrupleList.at(tmpIndex).resultIndex;
        // 回填操作
        newQuadrupleList.at(tmpIndex).resultIndex = quad;
        // 更新寻找下一条
        tmpIndex = nextQuadrupleIndex;
    } while (tmpIndex > 0);

//    while (tmpIndex != 0) {
//        // 目前正在回填的四元式的第四位即下一个需要回填的四元式的序号
//        int nextQuadrupleIndex = newQuadrupleList.at(tmpIndex).resultIndex;
//        // 回填操作
//        newQuadrupleList.at(tmpIndex).resultIndex = quad;
//        // 更新寻找下一条
//        tmpIndex = nextQuadrupleIndex;
//    }

}

void QuadrupleTranslator::checkError(int pos) {
    std::cerr << "存在语法错误\n";
    for (int lineCount = 0; lineCount < locateInputCode.size(); ++lineCount) {
        if (locateInputCode[lineCount] > pos) {
            cout << lineCount << endl;
            break;
        }
    }
    exit(0);
}

static void printStateStack(const stack<int>& stateStack) {
    cout << "状态栈 ";
    stack<int> copy = stateStack;
    stack<int> reversed;
    while(!copy.empty()) {
        reversed.push(copy.top());
        copy.pop();
    }
    while(!reversed.empty()) {
        cout << reversed.top() << " ";
        reversed.pop();
    }
    cout << endl;
}

static void printSymbolStack(const stack<Symbol>& symbolStack) {
    cout << "符号栈 ";
    stack<Symbol> copy = symbolStack;
    stack<Symbol> reversed;
    while(!copy.empty()) {
        reversed.push(copy.top());
        copy.pop();
    }
    while(!reversed.empty()) {
        cout << reversed.top().name << " ";
        reversed.pop();
    }
    cout << endl;
}

void QuadrupleTranslator::parse() {
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

    while(!symbolStack.empty()) {
        // 取状态栈栈顶的状态进行判断
        int curState = stateStack.top();
        // 读取输入串的下一个字符
        string symbolToRead; // 为了能读空字，所以这里先声明
        // TODO:                (这里不是什么未完成的，只是打个tag用思考) 如果这里是关系运算符是不是已经读进去了?就不需要后面if的操作了

        string nextSymbol = lexicalTable[inputPointer].token;
        cout << "";

        // FIXME: 读空字存在问题：句末为空的时候如果ACTION表里能读空字会去读空字，目前的临时办法是句末带#符号，但这肯定不对，需要解决
        // FIXME:  是不是应该只有shift才读空字？
        if (ActionTable[curState][VtToIndexMap["null"]] != ERROR &&
            !(lexicalTable[inputPointer].token == "#")) {
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
        /**
         * 语义处理① => 识别字符
         */
        // 变量或数字一起当做终结符i处理
        // 当前状态面临栈外输入串头部符号，ACTION表内的状态
        int curSymbolIndex = ActionTable[curState][VtToIndexMap[symbolToRead]];

        // 如果当前符号表位置为0(ERROR)，报错
        if (curSymbolIndex == ERROR) {
            cout << "【错误】在状态 " << curState << " 发生错误\n";
            checkError(inputPointer);
            exit(0);
        } else if (isShiftTerm(curSymbolIndex)) {
            cout << "【移进】从状态 " << curState << " 移进状态 " << getShiftStateIndex(curSymbolIndex) << endl;
            // 移进项目 => 直接入栈，转向新状态
            stateStack.push(getShiftStateIndex(curSymbolIndex));
            symbolStack.push(Symbol{symbolToRead});
            // 栈外指针指向下一位
            if (symbolToRead != "null")
                inputPointer++;
            printStateStack(stateStack);
            printSymbolStack(symbolStack);
        } else if (isReduceTerm(curSymbolIndex)) {
            cout << "【规约】在状态 " << curState << " 使用第 " << getReduceStateIndex(curSymbolIndex) << " 个产生式进行规约\n";
            // 规约项目 => 找到是用哪个串进行规约的
            int whichProductionToReduce = getReduceStateIndex(curSymbolIndex);
            // 被规约的串
            Term reduceTerm;
            // 当前产生式
            string production = productionTable[whichProductionToReduce];
            GrammarAnalyzer::split(production, reduceTerm.leftPart, reduceTerm.rightPart);

            /**
             * 语义处理② => 非终结符 => 非终结符类型单字符规约
             *
             * 从{用户自定义变量}->i->id->factor->term->neg->expr，
             * 然后才可以和其他变量进行比较运算{>,>=,==,<,<=,<>}
             * 所以这一长串规约方式相同
             */
            if (production == "<<FACTOR>>-><<ID>>"
                || production == "<<TERM>>-><<FACTOR>>"
                || production == "<<NEG>>-><<TERM>>"
                || production == "<<EXPR>>-><<NEG>>") {
                // 取栈顶串进行归约
                // CORE:{13,15,23,43}【语义处理】从简单变量i一路归约到表达式expression然后参与计算

                Symbol topSymbol = symbolStack.top();

                // ATTENTION: 如果是ID规约为FACTOR，还要考虑这个符号是否已经被定义为LABEL;如果未定义为LABEL则定义为VARIABLE
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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);
                symbolStack.top().place = topSymbol.place;
//                symbolStack.top().name = topSymbol.name;
                // TODO : TAG
                symbolStack.top().valString = topSymbol.valString;
                symbolStack.top().isInteger = topSymbol.isInteger;
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<ID>>->i") {
                /**
                 * 语义处理③ => 终结符归约为非终结符类型
                 *
                 * CORE:{17}【语义处理】由识别用户输入串到非终结符<<ID>>
                 */

                symbolStack.pop();
                stateStack.pop();
                // 将该被归约的非终结符进栈
                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                // 新状态进栈
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);
                // 记录为该符号的入口地址

                // valString记录的是终结符i的名字，名字为了在调试过程中能看懂归约到哪一步了所以保持<<ID>>不变
                symbolStack.top().place = entry[semanticStack.top()];
//                symbolStack.top().name = semanticStack.top();
                // TODO: TAG
                symbolStack.top().valString = semanticStack.top();
                bool isIncluded = false;
                for (const auto& symbol : symbolTable) {
                    if (symbol.name == symbolStack.top().name) {
                        symbolStack.top().isInteger = symbol.isInteger;
                        isIncluded = true;
                        break;
                    }
                }
                if (!isIncluded)
                    symbolStack.top().isInteger = symbolTable.back().isInteger;
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<RELOP>>->="
            || production == "<<RELOP>>->!="
            || production == "<<RELOP>>-><"
            || production == "<<RELOP>>-><="
            || production == "<<RELOP>>->>"
            || production == "<<RELOP>>->>=") {
                /**
                 * CORE:{27,28,29,30,31,32}【语义处理】读取关系运算符
                 */

                symbolStack.pop();
                stateStack.pop();
                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                // 这样就可以了吗？？
                // semanicStack存储了变量名和各种终结符,直接读取栈顶的符号名应该就是符号吧?
//                symbolStack.top().name = semanticStack.top();
//                symbolStack.top().name = reduceTerm.rightPart.at(0);
                symbolStack.top().val = semanticStack.top();
                // TODO : TO BE CHECKED:符号的valString也赋给上层relop
                symbolStack.top().valString = reduceTerm.rightPart.at(0);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<BCMP>>->(<<BEXPR>>)") {
                // CORE:带括号的布尔表达式归约
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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                symbolStack.top().trueExit = booleanExpr.trueExit;
                symbolStack.top().falseExit = booleanExpr.falseExit;

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "<<BCMP>>-><<EXPR>>") {
                // CORE:差点漏了这个单目布尔表达式。。。。
                Symbol booleanExpr = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                symbolStack.top().trueExit = nextQuad - offset;
                symbolStack.top().falseExit = nextQuad + 1 - offset;
                generateIntermediateCode("jnz", booleanExpr.valString, booleanExpr.isInteger, "-", false, 0);
                generateIntermediateCode("j", "-", false, "-", false, 0);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<BCMP>>-><<EXPR>><<RELOP>><<EXPR>>") {
                /**
                 * CORE:{4}     【语义处理】从两个表达式(expr)进行布尔运算归约到布尔类型比较表达式(bool_comparison)
                 */
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
                         GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                 // 给出真假出口地址
                 symbolStack.top().trueExit = nextQuad - offset;
                 symbolStack.top().falseExit = nextQuad + 1 - offset;

                 // 生成两条语句
                string place1Str = std::to_string(expression1.place);
                string place2str = std::to_string(expression2.place);
                generateIntermediateCode("j" + relop.valString, expression1.valString,
                                         expression1.isInteger, expression2.valString, expression2.isInteger, 0);
                generateIntermediateCode("j", "-", false, "-", false,0);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "A-><<ID>>:=<<EXPR>>") {
                /**
                 * 赋值语句: A -> ID := expression
                 *
                 * CORE:{1}     【语义处理】赋值语句：A->id:=E
                 */

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

//                cout << "id:=expr归约过程中,id的valstring是： " << id.valString << endl;
//                cout << "labelTable有 " << id.valString << "吗 => " << labelTable.count(id.valString) << endl;
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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);


                // 原版的这里是终结符i, 这里是非终结符<<ID>>,看了下上面的i规约为<<ID>>的步骤，
                // 感觉这样大概应该也行得通？
                semanticStack.pop();
                if (!tempVarStack.empty()) {
                    tempVarStack.pop();
                    semanticStack.pop();
                }
                id.name = semanticStack.top();
                // FIXME:      一个不知道为啥的危险举动：如果临时变量栈非空，这里就多pop一次，因为临时变量存储了两个变量相运算的值，而不是语义栈里的值

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
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "S->A"
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
                // 顶部语句
                Symbol statement = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                // 一个=赋所有属性
                // FIXME:           感觉是不是这里有问题啊,S->A规约后符号栈里的S又变成A了?虽然能正常work但是不适合debug用
//                symbolStack.top() = statement;
                symbolStack.top().place = statement.place;
                symbolStack.top().val = statement.val;
                symbolStack.top().valString = statement.valString;
                symbolStack.top().trueExit = statement.trueExit;
                symbolStack.top().falseExit = statement.falseExit;

                if (production == "S->A") {
                    // TODO: 这里和N规约的语义动作是一样的
                    // => FIXME: 修正:我觉得你啥也不做就行(PPT上的makelist动作把我害惨了= =)
//                    symbolStack.top().nextList = nextQuad - offset;
                } else if (production == "L->S") {
                    symbolStack.top().nextList = statement.nextList;
                }

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<EXPR>>-><<EXPR>>+<<TERM>>"
            || production == "<<TERM>>-><<TERM>>*<<FACTOR>>") {
                // CORE:{14,44}     【语义处理】算术运算：从'+'运算归约到表达式(expression) or 从'*"运算归约到表达式

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
                // TODO: TO BE CHECKED  这里改了初次的valstring
                tempVar.valString = expression1.valString + reduceTerm.rightPart[1] + expression2.valString;
                // FIXME:               在归约的时候直接计算有bug（本身就存在变量没赋值的情况），如果只是生成代码的话也可以不考虑哈。后面回填的时候再计算结果
//                try {
//                    int val1 = std::stoi(symbolTable[expression1.place].val);
//                    int val2 = std::stoi(symbolTable[expression2.place].val);
//                    if (reduceTerm.rightPart[1] == "+")
//                        tempVar.val = std::to_string(val1 + val2);
//                    else if (reduceTerm.rightPart[1] == "*")
//                        tempVar.val = std::to_string(val1 * val2);
//                } catch (...) {
//                    checkError(inputPointer);
//                    exit(0);
//                }
                // 临时变量存入符号表，entry表记录临时变量地址
                symbolTable.push_back(tempVar);
                entry[tempVar.name] = tempVar.place;
                // 临时变量存入临时变量表 => 可能会产生冗余的临时变量，不管了。。
                tempVarStack.push(tempVar);

                Symbol newLeft = Symbol{reduceTerm.leftPart};
                // FIXME:               不保证这个地方没有问题，只是对于单个a:=b+c的产生式没问题
                newLeft.valString = tempVar.name;
//                newLeft.valString = tempVar.valString;
                symbolStack.push(newLeft);
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);



                generateDoubleArgThreeAddressCode(tempVar.name, reduceTerm.rightPart[1], expression1.valString,
                                                  expression1.isInteger, expression2.valString, expression2.isInteger);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<NEG>>->-<<TERM>>") {
                // CORE:{24}【语义处理】取负运算

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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

//                string expr = "uminus " + expression.valString;
                generateSingleArgThreeAddressCode(tempVar.name, "uminus", expression.valString, expression.isInteger);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<FACTOR>>->(<<EXPR>>)") {
                // CORE:{16} 【语义处理】括号运算
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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                symbolStack.top().place = expression.place;
//                symbolStack.top().name = expression.name;
                symbolStack.top().valString = expression.valString;
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<BEXPR>>-><<BAND>>"
            || production == "<<BAND>>-><<BNOT>>"
            || production == "<<BNOT>>-><<BCMP>>") {
                // CORE: {2,7,9}【语义分析】布尔表达式递归归约

                // 记录产生式右部符号
                Symbol rightSymbol = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                // 布尔表达式，传递真假出口
                symbolStack.top().trueExit = rightSymbol.trueExit;
                symbolStack.top().falseExit = rightSymbol.falseExit;
                // 下面这句不知道啥意思，但加上了
                symbolStack.top().valString = rightSymbol.valString;
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "<<OPENSTMT>>->if<<BEXPR>>thenM<<STMT>>") {
                // CORE:{25}【语义分析】if E then M S 条件语句
                // 符号栈和状态栈弹出五次，记录BEXPR, M和STMT
                Symbol booleanExpression, M, statement;
                for(int count = 0; count < 5; ++count) {
                    if (count == 0)
                        statement = symbolStack.top();
                    else if (count == 1)
                        M = symbolStack.top();
                    else if (count == 3)
                        booleanExpression = symbolStack.top();
                    symbolStack.pop();
                    stateStack.pop();
                }

                // 常规操作
                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                // 回填和合并链表
                backPatch(booleanExpression.trueExit, M.quad);
                symbolStack.top().nextList = merge(booleanExpression.falseExit, statement.nextList);

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "S->if<<BEXPR>>thenMSNelseMS"
            || production == "<<OPENSTMT>>->if<<BEXPR>>thenMSNelseM<<OPENSTMT>>") {
                // CORE:{26,33}【语义分析】if then else语句的翻译

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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                backPatch(booleanExpr.trueExit, M1.quad);
                backPatch(booleanExpr.falseExit, M2.quad);
                symbolStack.top().nextList = merge(statement1.nextList, N.nextList, statement2.nextList);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "M->null"
            || production == "N->null") {
                //CORE{21,22}【语义分析】规约为空字

                // 符号栈不需要弹出任何符号，而是直接凭空读进去一个M或者N
                // ATTENTION:↑ 错了， 要把"null"这个终结符弹出来
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

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
                    generateIntermediateCode("j", "-", false, "-", false, 0);
                }

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "L->L;MS") {
                //CORE:{18}

                Symbol L, M, statement;
                for(int count = 0; count < 4; ++count) {
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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                backPatch(L.nextList, M.quad);
                L.nextList = statement.nextList;

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "S->{L}") {
                //CORE:{35}

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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                symbolStack.top().nextList = L.nextList;

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "<<LABEL>>-><<ID>>:") {
                // CORE:{20} LABEL语句
                symbolStack.pop();
                stateStack.pop();
                Symbol label = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

//                cout << "当前被归约为label的id的valstring:" << label.valString << endl;

                // ATTENTION:如果label已经被定义为variable则报错
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
//                    cout << "新定义label " << label.valString << " : 指向地址：" << nextQuad << endl;
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "S-><<LABEL>>S") {
                //CORE:{38}【语义分析】label语句被归约为语句

                Symbol statement = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "S->goto<<ID>>") {
                //CORE:{39}【语义分析】goto语句

                Symbol label = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);




                int address = -1;
//                cout << "第一次出现goto:" << label.valString << " " << labelTable.count(label.valString) << endl;
                if (labelTable.count(label.valString) == 0) {
//                    std::cerr << "跳转到未定义的label " << label.valString << endl;
//                    checkError(0);
                    labelTable[label.valString].nextList =
                            merge(labelTable[label.valString].nextList,
                                  nextQuad - offset);
                } else {
//                    cout << "label "<< label.valString <<"出现的次数=0吗？" << labelTable.count(label.valString) << endl;
                    // 如果该label的跳转地址已确定，则跳转到该地址
                    if (labelTable[label.valString].quad != -1) {
                        address = labelTable[label.valString].quad;
//                        newQuadrupleList.back().resultIndex = labelTable[label.valString].quad;
                    }
                    // 否则将该地址添加到label的nextList里
                    else
//                        labelTable[label.valString].nextList =
//                                merge(labelTable[label.valString].nextList,
//                                      nextQuad - offset);
                    labelTable[label.valString].nextList =
                            merge(nextQuad - offset,
                                  labelTable[label.valString].nextList);
                }

                generateIntermediateCode("j", "-", false, "-", false, address);


                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            }
            else if (production == "S->whileM<<BEXPR>>doMS") {
                //CORE:{36}【语义分析】while语句 S->whileM<<BEXPR>>doMS

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

                //常规
                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                //回填
                backPatch(statement.nextList, M1.quad);
                backPatch(booleanExpr.trueExit, M2.quad);
                symbolStack.top().nextList = booleanExpr.falseExit;

                generateIntermediateCode("j", "-", false, "-", false, M1.quad);

                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "S->call<<ID>>(<<ELIST>>)") {
                //CORE:{37}【语义分析】函数调用

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
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                Procedure function = procedureList.back();
                function.name = id.valString;
                for (const auto& param : function.parameters)
                    generateProcedureCall(true, "param " + param);
                generateProcedureCall(true, "call " + function.name);

                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "<<ELIST>>-><<EXPR>>") {
                //CORE:{11}【语义分析】构造过程调用的参数

                Symbol expr = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                Procedure procedure;
                procedure.parameters.push_back(expr.valString);
                procedureList.push_back(procedure);

                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "<<ELIST>>-><<ELIST>>,<<EXPR>>") {
                //CORE:{12}【语义分析】构造过程调用的参数

                Symbol expr = symbolStack.top();
                for (int count = 0; count < 3; ++count) {
                    symbolStack.pop();
                    stateStack.pop();
                }

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndexMap[symbolStack.top().name]]);

                procedureList.back().parameters.push_back(expr.valString);

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

void QuadrupleTranslator::lex() {
    string inputCode;
    std::ifstream inFile("input.txt");
    cout << "------------------中间代码生成----------------------\n";
    locateInputCode.push_back(0);
    while (std::getline(inFile, inputCode)) {
        cout << inputCode << endl;
        // ATTENTION:句末读空字问题已解决：语法分析的时候手动在输入串末尾添加一个#
        inputCode.push_back('#');
//        cout << "现在正在语法解析 " << inputCode << endl;
        Tokenizer::lexicalAnalyze(inputCode);
        locateInputCode.push_back(lexicalTableLength);
    }

    cout << "\nlexical table: \n";
    for (const auto& word : lexicalTable)
        word.print();

}


#endif //INTERMEDIATE_CODE_GENERATOR_QUADRUPLETRANSLATOR_H
