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

class Symbol {
public:
    string name;
    string val{"0"};
    string valString; // 这个属性是生成三地址代码用的
    int PLACE{};
    int trueExit{};
    int falseExit{};

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

vector<Quadruple> quadrupleList;
vector<string> quadrupleOrTAC; // quadruple(四元式)或者TAC(Three Address Code, 三地址代码)
const int offset = 10000;
int nextQuad = 0 + offset;
vector<Symbol> symbolTable;
map<string, int> entry;
int tempVariableCount = 0;

Symbol Symbol::generateNewTempVar() {
    tempVariableCount++;
    return Symbol{"T"+std::to_string(tempVariableCount)};
}

class QuadrupleTranslator {
public:

    static void generateIntermediateCode(const string& op, const string& arg1, const string& arg2, int resultIndex);

    static void generateThreeAddressCode(const string& result, const string& expression);

    static void generateFinalIntermediateCode();

    static int merge(int l1, int l2);

    static void backpatch(int head, int t);

    static void checkError(int pos);

    // 语义分析并生成中间代码
    static void parse();

    // 读取文件并生成四元式
    static void lex();
};

void QuadrupleTranslator::generateIntermediateCode(const string& op, const string& arg1, const string& arg2, int resultIndex) {
    quadrupleList.push_back(Quadruple{op, arg1, arg2, resultIndex});
    char result[256];
    std::sprintf(result, "%d", resultIndex);
    quadrupleOrTAC.push_back("(" + op + ", " + arg1 + ", " + arg2 + ", " + result + ")");
}

void QuadrupleTranslator::generateFinalIntermediateCode() {

}

int QuadrupleTranslator::merge(int list1, int list2) {
    return 0;
}

void QuadrupleTranslator::backpatch(int truelist, int quad) {

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
    while(!copy.empty()) {
        cout << copy.top() << " ";
        copy.pop();
    }
    cout << endl;
}

static void printSymbolStack(const stack<Symbol>& symbolStack) {
    cout << "符号栈 ";
    stack<Symbol> copy = symbolStack;
    while(!copy.empty()) {
        cout << copy.top().name << " ";
        copy.pop();
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

    generateIntermediateCode("_", "-", "-", offset);
    int oldPointer = -1; // 存入上一次操作的输入串指针

    while(!symbolStack.empty()) {
        // 取状态栈栈顶的状态进行判断
        int curState = stateStack.top();
        // 读取输入串的下一个字符
        string symbolToRead = lexicalTable[inputPointer].token;

        // TODO:                (这里不是什么未完成的，只是打个tag用思考) 如果这里是关系运算符是不是已经读进去了?就不需要后面if的操作了

        /**
         * 语义处理① => 识别字符
         */
        // 变量或数字一起当做终结符i处理
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
                    tmpSymbol.PLACE = symbolTable.size();
                    symbolTable.push_back(tmpSymbol);
                    entry[tmpSymbol.name] = tmpSymbol.PLACE;
                }
                // 符号表其中已经有的符号不进行上述处理
            }
            // 更新指针
            oldPointer = inputPointer;
        }
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
                symbolStack.top().name = topSymbol.name;
                // TODO : TAG
                symbolStack.top().valString = topSymbol.valString;
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
                symbolStack.top().PLACE = entry[semanticStack.top()];
                // TODO:            下一句我也没有把握，直接从语义栈里拿第一个词出来代表被归约的用户自定义变量名了。 => 目前来看结果似乎应该是对的。
                symbolStack.top().name = semanticStack.top();
                // TODO: TO BE CHECKED: i归约为<<ID>>时，ID的valString赋为i的name
                symbolStack.top().valString = semanticStack.top();
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
                symbolStack.top().name = semanticStack.top();
                symbolStack.top().val = semanticStack.top();
                // TODO : TO BE CHECKED:符号的valString也赋给上层relop
                symbolStack.top().valString = semanticStack.top();
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
                 symbolStack.top().trueExit = nextQuad;
                 symbolStack.top().falseExit = nextQuad + 1;

                 // 生成两条语句
                string PLACE1str = std::to_string(expression1.PLACE);
                string PLACE2str = std::to_string(expression2.PLACE);
                generateIntermediateCode("j" + relop.name, PLACE1str, PLACE2str, 0);
                generateIntermediateCode("j", "-", "-", 0);
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
                // TODO:      因为实验手册要求的是用三地址代码。。。但是跳转没法三地址代码，所以就 ①赋值用三地址代码 ②跳转四元式 混用吧。
                // generateIntermediateCode(":=", PLACEstr, "-", id.PLACE);
                generateThreeAddressCode(id.valString, expression.valString);
                // 调试用
                cout << " => 规约后的新状态是 " << stateStack.top() << endl;
                printStateStack(stateStack);
                printSymbolStack(symbolStack);
            } else if (production == "S->A"
            || production == "<<STMT>>->S"
            || production == "<<START>>-><<STMT>>") {
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
                symbolStack.push(newLeft);
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);



                // 生成三地址代码
                string expr = expression1.valString + " " + reduceTerm.rightPart[1] + " " + expression2.valString;
                generateThreeAddressCode(tempVar.name, expr);
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

                string expr = "uminus " + expression.valString;
                generateThreeAddressCode(tempVar.name, expr);
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
                symbolStack.top().name = expression.name;
                symbolStack.top().valString = expression.valString;
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
            cout << "该表达式能被成功分析, ACCEPT" << endl;
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
        Tokenizer::lexicalAnalyze(inputCode);
        locateInputCode.push_back(lexicalTableLength);
    }

    cout << "\nlexical table: \n";
    for (const auto& word : lexicalTable)
        word.print();

}

void QuadrupleTranslator::generateThreeAddressCode(const string& result, const string& expression) {
    quadrupleOrTAC.push_back(result + " := " + expression);
}

#endif //INTERMEDIATE_CODE_GENERATOR_QUADRUPLETRANSLATOR_H
