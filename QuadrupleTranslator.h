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

struct Symbol {
    string name;
    string val{"0"};
    int PLACE{};
    int trueExit{};
    int falseExit{};

    Symbol newTemp();
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

Symbol Symbol::newTemp() {
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

void QuadrupleTranslator::parse() {
    // 初始化状态栈/符号栈/语义栈
    stack<int> stateStack; // 状态栈
    stack<Symbol> symbolStack; // 符号栈
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

        // TODO:(这里不是什么未完成的，只是打个tag用思考) 如果这里是关系运算符是不是已经读进去了?就不需要后面if的操作了

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
            checkError(inputPointer);
            exit(0);
        } else if (isShiftTerm(curSymbolIndex)) {
            // 移进项目 => 直接入栈，转向新状态
            stateStack.push(getShiftStateIndex(curSymbolIndex));
            symbolStack.push(Symbol{symbolToRead});
            // 栈外指针指向下一位
            inputPointer++;
        } else if (isReduceTerm(curSymbolIndex)) {
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
            } else if (production == "<<ID>>->i") {
                /**
                 * 语义处理③ => 终结符归约为非终结符类型
                 *
                 * 由识别用户输入串到非终结符<<ID>>
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
                // TODO: 下一句我也没有把握，直接从语义栈里拿第一个词出来代表被归约的用户自定义变量名了。 => 目前来看结果似乎应该是对的。
                symbolStack.top().name = semanticStack.top();
            } else if (production == "<<RELOP>>->="
            || production == "<<RELOP>>->!="
            || production == "<<RELOP>>-><"
            || production == "<<RELOP>>-><="
            || production == "<<RELOP>>->>"
            || production == "<<RELOP>>->>=") {
                /**
                 * 读取关系运算符
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

            } else if (production == "<<BCMP>>-><<EXPR>><<RELOP>><<EXPR>>") {


                /**
                 * 从这里往下是一个完整的if E then M S的流程: => if a<b then c:=d
                 * 包含
                 * ① 布尔表达式(a<b)
                 * ② 赋值语句(c:=d)
                 * ③ 条件语句(if E then M S)
                 */



                /**
                 * 语义处理④ => 语句 => 布尔表达式
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
                 char *PLACE2str;
                char *PLACE1str;
                std::sprintf(PLACE1str, "%d", expression1.PLACE);
                 std::sprintf(PLACE2str, "%d", expression2.PLACE);
                generateIntermediateCode("j" + relop.name, PLACE1str, PLACE2str, 0);
                generateIntermediateCode("j", "-", "-", 0);
            } else if (production == "A-><<ID>>:=<<EXPR>>") {
                /**
                 * 赋值语句: A -> ID := expression
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
                id.name = semanticStack.top();
                semanticStack.pop();
                // 更新符号的入口地址
                id.PLACE = entry[id.name];
                // 值更新为表达式的值(的入口地址)
                id.val = symbolTable[expression.PLACE].val;
                // 同时更新symbolTable
                symbolTable[id.PLACE].val = symbolTable[expression.PLACE].val;

                // 产生一句赋值语句
                char *PLACEstr;
                std::sprintf(PLACEstr, "%d", expression.PLACE);
                // TODO: 因为实验手册要求的是用三地址代码。。。但是跳转没法三地址代码，所以就 ①赋值用三地址代码 ②跳转四元式 混用吧。
                // generateIntermediateCode(":=", PLACEstr, "-", id.PLACE);
                generateThreeAddressCode(id.name, expression.name);
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

                // 顶部语句
                Symbol statement = symbolStack.top();
                symbolStack.pop();
                stateStack.pop();

                symbolStack.push(Symbol{reduceTerm.leftPart});
                curState = stateStack.top();
                stateStack.push(
                        GotoTable[curState][VnToIndex[symbolStack.top().name]]);

                // 一个=赋所有属性
                symbolStack.top() = statement;
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
