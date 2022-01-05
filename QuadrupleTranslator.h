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

class newQuadruple;
class Quadruple;
class Symbol;

vector<newQuadruple> newQuadrupleList;

vector<Quadruple> quadrupleList;
vector<string> quadrupleOrTAC; // quadruple(四元式)或者TAC(Three Address Code, 三地址代码)
const int offset = 100;
int nextQuad = 0 + offset;
vector<int> addresses; // 四元式地址表
vector<Symbol> symbolTable;
map<string, int> entry;
int tempVariableCount = 0;


class Symbol {
public:
    string name;
    string val{"0"};
    string valString; // 这个属性是生成三地址代码用的
    bool isInteger{false};
    int PLACE{-1};
    int trueExit{-1}; // 真出口
    int falseExit{-1}; // 假出口
    int nextList{-1}; // 需要在确定自己的下一条四元式的实际地址后，进行回填的四元式序列
    int quad{-1}; // 下一条语句的地址

    static Symbol generateNewTempVar();
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

    void setResultIndex(int newIndex) { resultIndex = newIndex; }

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

    void print() const {
        if (isAssignment) {
            if (hasArg2)
                cout << resultName << " := " << arg1 << " " << op << " " << arg2 << endl;
            else if (op == "uminus")
                cout << resultName << " := uminus " << arg1 << endl;
            else
                cout << resultName << " := " << arg1 << endl;
        }
        else
            cout << "(" << op << ", " << arg1Index << ", " << arg2Index << ", "
            << (resultIndex < offset ? "uncertain" : std::to_string(resultIndex))
            << ")" << endl;
    }

    void printArgInName() const {
        if (isAssignment) {
            if (hasArg2)
                cout << resultName << " := " << arg1 << " " << op << " " << arg2 << endl;
            else if (op == "uminus")
                cout << resultName << " := uminus " << arg1 << endl;
            else
                cout << resultName << " := " << arg1 << endl;
        } else
            cout << "(" << op << ", " << arg1 << ", " << arg2 << ", "

            // 调试用：打印假地址(待回填的四元式的序号而非真实跳转地址)
//            << resultIndex
            // 调试用：未确定的跳转地址全部填成最后一条四元式的下一条四元式
            << (resultIndex < offset ? nextQuad : resultIndex)

            // 下面这条才是真实的跳转地址
//            << (resultIndex < offset ? "uncertain" : std::to_string(resultIndex))
            << ")" << endl;
    }

    void printAsQuadruple() const {
        if (isAssignment) {
            if (hasArg2)
                cout << "(" << op << ", " << (isArg1Integer ? "#" : "") << arg1 << ", "
                << (isArg2Integer ? "#" : "") << arg2 << ", " << resultName << ")" << endl;
            else if (op == "uminus")
                cout << "(" << "uminus" << ", " << (isArg1Integer ? "#" : "") << arg1 << ", -, " << resultName << ")" << endl;
            else
                cout << "(" << op << ", " << (isArg1Integer ? "#" : "") << arg1 << ", -, " << resultName << ")" << endl;
        } else
            cout << "(" << op << ", " << (isArg1Integer ? "#" : "") << arg1 << ", "
            << (isArg2Integer ? "#" : "") << arg2 << ", "

              // 调试用：打印假地址(待回填的四元式的序号而非真实跳转地址)
//              << resultIndex
              // 调试用：未确定的跳转地址全部填成最后一条四元式的下一条四元式
              << (resultIndex < offset ? nextQuad : resultIndex)

              // 下面这条才是真实的跳转地址
//            << (resultIndex < offset ? "uncertain" : std::to_string(resultIndex))
            << ")" << endl;
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

    static void generateQuadruple(const string& op, const string& arg1Name, bool isArg1Integer, const string& arg2Name, bool isArg2Integer, int resultIndex);

    static void generateFinalIntermediateCode();

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
    nextQuad++;
}

void QuadrupleTranslator::generateQuadruple(const string &op, const string &arg1Name, bool isArg1Integer, const string &arg2Name,
                                            bool isArg2Integer, int resultIndex) {

    newQuadruple quadruple(op, false, arg1Name, 0, isArg1Integer, arg2Name, 0, isArg2Integer, std::to_string(resultIndex), resultIndex);
    newQuadrupleList.push_back(quadruple);
    nextQuad++;
}

void QuadrupleTranslator::generateSingleArgThreeAddressCode(const string &result, const string &op, const string &arg, bool isArgInteger) {
    newQuadruple quadruple(op, arg, isArgInteger, result);
    newQuadrupleList.push_back(quadruple);
    nextQuad++;
}


void QuadrupleTranslator::generateFinalIntermediateCode() {

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
    // list2为空，直接返回list1的表头
//    if (listHead2 == offset || listHead2 == -1 || listHead2 == 0) {
//        return listHead1;
//    }
    else {
        // 递归寻找表头：每个跳转四元式的第四位在填写真正的跳转地址前，
        // 填写的是等待被回填的所有四元式组成的链表中的上一个四元式的序号。
        // 该链表的表头因为没有上一个四元式了，所以第四位填写的是第一个地址即offset，
        // 因此一直找到最后一位为offset的四元式，就是该链表的表头。
        int tempHead = listHead2;
        while (newQuadrupleList.at(tempHead).resultIndex != 0) {
            tempHead = newQuadrupleList.at(tempHead).resultIndex;
            // FIXME: 以下是强行跳出循环的意义不明语句
        }
        // 此时tempHead指向的已经是表头，把表头指向的地址指向list1的链尾，完成合并操作。
        newQuadrupleList.at(tempHead).resultIndex = listHead1;
        return listHead2;
    }
}

int QuadrupleTranslator::merge(int listHead1, int listHead2, int listHead3) {
    // 我想复杂了，调用二参数的merge递归一下就行了
    return merge(listHead1, merge(listHead2, listHead3));
}


// 回填，在得到一支跳转地址相同但未确定的四元式的确定跳转地址后，用该地址回填这些四元式
void QuadrupleTranslator::backPatch(int listHead, int quad) {
    if (listHead == -1 || quad == 0)
        return;

    int tmpIndex = listHead;

//    while (tmpIndex != offset) {
//        // 目前正在回填的四元式的第四位即下一个需要回填的四元式的序号
//        int nextQuadrupleIndex = newQuadrupleList.at(tmpIndex - offset).resultIndex + offset;
//        // 回填操作
//        newQuadrupleList.at(tmpIndex - offset).resultIndex = quad;
//        // 更新寻找下一条
//        tmpIndex = nextQuadrupleIndex;
//    }

    // TODO: 对于链首也需要回填的地方，while会跳过第一个需要回填的式子，改成了do while多一次循环试试看
    do {
        // 目前正在回填的四元式的第四位即下一个需要回填的四元式的序号
        int nextQuadrupleIndex = newQuadrupleList.at(tmpIndex).resultIndex;
        // 回填操作
        newQuadrupleList.at(tmpIndex).resultIndex = quad;
        // 更新寻找下一条
        tmpIndex = nextQuadrupleIndex;
    } while (tmpIndex != 0);

}

void QuadrupleTranslator::checkError(int pos) {
    std::cerr << "存在语法错误\n";
    for (int lineCount = 0; lineCount < locateInputCode.size(); ++lineCount) {
        if (locateInputCode[lineCount] > pos) {
            cout << lineCount << endl;
            break;
        }
    }
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
        if (ActionTable[curState][VtToIndex["null"]] != ERROR &&
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
                        tmpSymbol.PLACE = symbolTable.size();
                        symbolTable.push_back(tmpSymbol);
                        entry[tmpSymbol.name] = tmpSymbol.PLACE;
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
        int curSymbolIndex = ActionTable[curState][VtToIndex[symbolToRead]];

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
                symbolStack.pop();
                stateStack.pop();
                // 规约的产生式左部进栈
                symbolStack.push(Symbol{reduceTerm.leftPart});
                // 根据现在栈顶状态计算状态转移
                curState = stateStack.top();
                // 新状态进栈
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);
                symbolStack.top().PLACE = topSymbol.PLACE;
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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);
                // 记录为该符号的入口地址

                // valString记录的是终结符i的名字，名字为了在调试过程中能看懂归约到哪一步了所以保持<<ID>>不变
                symbolStack.top().PLACE = entry[semanticStack.top()];
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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

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
                 * 从这里往下是一个完整的if E then M S的流程: => if a<b then c:=d
                 * 包含
                 * ① 布尔表达式(a<b)
                 * ② 赋值语句(c:=d)
                 * ③ 条件语句(if E then M S)
                 */



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
                         GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                 // 给出真假出口地址
                 symbolStack.top().trueExit = nextQuad - offset;
                 symbolStack.top().falseExit = nextQuad + 1 - offset;

                 // 生成两条语句
                string PLACE1str = std::to_string(expression1.PLACE);
                string PLACE2str = std::to_string(expression2.PLACE);
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

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);


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
                id.PLACE = entry[id.name];
                // 值更新为表达式的值(的入口地址)
                id.val = symbolTable[expression.PLACE].val;
                // 同时更新symbolTable
                symbolTable[id.PLACE].val = symbolTable[expression.PLACE].val;

                // 产生一句赋值语句

                string PLACEstr = std::to_string(expression.PLACE);
                generateSingleArgThreeAddressCode(id.valString, ":=", expression.valString, expression.isInteger);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "S->A"
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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 一个=赋所有属性
                // FIXME:           感觉是不是这里有问题啊,S->A规约后符号栈里的S又变成A了?虽然能正常work但是不适合debug用
//                symbolStack.top() = statement;
                symbolStack.top().PLACE = statement.PLACE;
                symbolStack.top().val = statement.val;
                symbolStack.top().valString = statement.valString;
                symbolStack.top().trueExit = statement.trueExit;
                symbolStack.top().falseExit = statement.falseExit;
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
                tempVar.PLACE = symbolTable.size();
                // TODO: TO BE CHECKED  这里改了初次的valstring
                tempVar.valString = expression1.valString + reduceTerm.rightPart[1] + expression2.valString;
                // FIXME:               在归约的时候直接计算有bug（本身就存在变量没赋值的情况），如果只是生成代码的话也可以不考虑哈。后面回填的时候再计算结果
//                try {
//                    int val1 = std::stoi(symbolTable[expression1.PLACE].val);
//                    int val2 = std::stoi(symbolTable[expression2.PLACE].val);
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
                entry[tempVar.name] = tempVar.PLACE;
                // 临时变量存入临时变量表 => 可能会产生冗余的临时变量，不管了。。
                tempVarStack.push(tempVar);

                Symbol newLeft = Symbol{reduceTerm.leftPart};
                // FIXME:               不保证这个地方没有问题，只是对于单个a:=b+c的产生式没问题
                newLeft.valString = tempVar.name;
//                newLeft.valString = tempVar.valString;
                symbolStack.push(newLeft);
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);



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
                tempVar.PLACE = symbolTable.size();
                tempVar.valString = "uminus " + expression.valString;

                symbolTable.push_back(tempVar);
                entry[tempVar.name] = tempVar.PLACE;

                tempVarStack.push(tempVar);

                Symbol newLeft = Symbol{reduceTerm.leftPart};
                newLeft.valString = tempVar.name;
                symbolStack.push(newLeft);
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                symbolStack.top().PLACE = expression.PLACE;
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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 回填和合并链表
                backPatch(booleanExpression.trueExit, M.quad);
                symbolStack.top().nextList = merge(booleanExpression.falseExit, statement.nextList);

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            } else if (production == "S->if<<BEXPR>>thenMSNelseMS"
            || production == "<<OPENSTMT>>->if<<BEXPR>>thenMSNelseM<<OPENSTMT>>") {
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
                // FIXME: 以下merge函数有点问题，先注释掉看能不能正常生成语句
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
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 接下来是对于M和N的两种不同处理
                // quad记录的是经偏移后的真实地址，
                // truelist/falselist/nextlist记录的是减去偏移的虚拟地址，
                // 虚拟地址小于偏移量100的意思就是还没有确定真实的地址。
                if (reduceTerm.leftPart == "M") {
                    // M是记录nextquad
                    symbolStack.top().quad = nextQuad;
                } else if (reduceTerm.leftPart == "N") {
                    // N的nextlist记录nextquad，意思是N产生的四元式的地址等待回填
                    symbolStack.top().nextList = nextQuad - offset;
                    generateIntermediateCode("j", "-", false, "-", false, 0);
                }

                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);

            }
            /**
             * 还没完。。但是先把结束态写了。
             */
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
