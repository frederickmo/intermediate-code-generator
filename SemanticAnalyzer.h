//
// Created by Frederick Mo on 2022/1/1.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_SEMANTICANALYZER_H
#define INTERMEDIATE_CODE_GENERATOR_SEMANTICANALYZER_H

#define ACCEPT_BASE 1000
#define SHIFT_BASE 2000
#define REDUCE_BASE 3000
#define CEILING 4000
#define ERROR 0


#include <utility>
#include <queue>
#include <fstream>
#include <sstream>

#include "GrammarAnalyzer.h"

using std::queue;

// 判断是否是移进项目
static bool isShiftTerm(int index) {
    return index >= SHIFT_BASE && index < REDUCE_BASE;
}

// 是否是规约项目
static bool isReduceTerm(int index) {
    return index >= REDUCE_BASE && index < CEILING;
}

// 是否是接受项目
static bool isAcceptTerm(int index) {
    return index == ACCEPT_BASE;
}

// 获取移进后的下一个状态索引
static int getShiftStateIndex(int index) {
    return index - SHIFT_BASE;
}

// 获取按第几个产生式进行归约的索引
static int getReduceStateIndex(int index) {
    return index - REDUCE_BASE;
}

enum TYPE {
    REDUCE,
    SHIFT,
    TO_REDUCE,
    ACCEPT
}type;

// 项目集：移进项目/待约项目/规约项目
struct Term {
    TYPE type{};
    string leftPart; // 左部
    vector<string> rightPart; // 右部
    int dotPos{-1}; // dotPos: 点在哪个符号前面，index就是谁
    vector<string> subsequence; // 展望符
    bool operator==(const Term& t) const {
        if (leftPart == t.leftPart
                && rightPart == t.rightPart
                && dotPos == t.dotPos
                && subsequence == t.subsequence)
            return true;
        else
            return false;
    }
};


static const int maxTermCount = 4000;
vector<Term> stateSet[maxTermCount];
int globalStateCount = 1;
int ActionTable[maxTermCount][60];
int GotoTable[maxTermCount][60];


class SemanticAnalyzer {
public:

//    GrammarAnalyzer grammarAnalyzer;

//    map<string, int> VtToIndex = grammarAnalyzer.VtToIndex;
//    map<string, int> VnToIndex = grammarAnalyzer.VnToIndex;

//    vector<string> productionTable;

    // 判断新生成的状态集是否可以和之前的状态集合并，
    // 若可以合并则返回状态集序号，不可合并返回-1
    static int mergeSet() ;

    // 分析第一个产生式，产生第一个状态集I0
    static void generateI0();

    // 计算状态集闭包
    static void calculateClosure(int stateIndex);

    // 计算状态集遇到某符号应该转换到的新状态的序号
    static int GOTO(int stateIndex, const string& symbol);



//    void initProductionTable(vector<string> table) {
//        this->productionTable = std::move(table);
//    }

    void printStateSet();

    void printStateTable();

    static void printStateTableToFile(const string& fileName);

    // 输出纯数字表，读入方便
    static void printStateTableToFilePureNumber(const string& fileName);

    static void readStateTable(const string& fileName);

    // 这里是用LR(1)分析法构造的状态集
    static void generateStateSet(int choice = 0);

    // 似乎LALR就是从LR(1)里合并了同心集就完了
    static void generateLALRTable();
};

int SemanticAnalyzer::mergeSet() {
    int flag = -1; // 默认不能合并

    for (int i = 0; i < globalStateCount - 1; ++i) {
        // 状态集项目数不等显然不能合并
        if (stateSet[globalStateCount - 1].size() != stateSet[i].size())
            continue;
        // 假设能和Ii项目集合并
        flag = i;
        for (int j = 0; j < stateSet[globalStateCount - 1].size(); ++j) {
            if (!(stateSet[i][j] == stateSet[globalStateCount - 1][j])) {
                flag = -1;
                break;
            }
        }
        if (flag != -1)
            return flag;
    }
    return -1;
}

void SemanticAnalyzer::generateI0() {
    Term firstTerm;
    string X;
    vector<string> Yset;
    GrammarAnalyzer::split(productionTable[0], X, Yset);
    if (firstTerm.dotPos == -1) // 未设置活前缀位置，就设置为最左
        firstTerm.dotPos = 0;
    firstTerm.leftPart = X;
    firstTerm.rightPart = Yset;
    firstTerm.subsequence.emplace_back("#");
    stateSet[0].push_back(firstTerm);
}

// 计算LR(0) / SLR / LR(1) 状态集的闭包
void SemanticAnalyzer::calculateClosure(int stateIndex) {

    queue<Term> termQueue;

    // 先将状态集的所有状态入队列
    for (auto & i : stateSet[stateIndex])
        termQueue.push(i);

    while(!termQueue.empty()) {
        Term curTerm = termQueue.front();

        // 点在最后 => 规约项，不会拉入新的项目，跳过
        if (curTerm.dotPos == curTerm.rightPart.size()) {
            termQueue.pop();
            continue;
        }

        // 若点右边有非终结符，则将该非终结符在左边的产生式拉入项目集
        string newLeftPart = curTerm.rightPart[curTerm.dotPos];
        for (auto & j : productionTable) {
            // newLeftPart不是该产生式的左部，continue
            string vn = GrammarAnalyzer::getVn(j.substr(0, j.length()));
            if (newLeftPart != GrammarAnalyzer::getVn(
                    j.substr(
                            0, j.length())))
                continue;

            Term newTerm;
            newTerm.dotPos = 0;
            // 将第j个产生式的左右部拆开放入newTerm中
            GrammarAnalyzer::split(j,newTerm.leftPart, newTerm.rightPart);

            // 项目右部点右边第一个符号是终结符： B->•bA类型，则为移进(SHIFT)项目
            if (VtToIndex.count(newTerm.rightPart[0]) != 0)
                newTerm.type = SHIFT;
            // 项目右部点右边第一个符号是非终结符：B->b•A类型，则为待约(TO_REDUCE)项目
            else if (VnToIndex.count(newTerm.rightPart[0]) != 0)
                newTerm.type = TO_REDUCE;

            string newSubsequence;
            vector<string> newSubsequenceSet;
            // 如果拉入新的项目时，•右边的非终结符右边还有新的符号，
            // 即要归约出该非终结符时，应该期待下一个符号，
            // 下一个符号就是新的展望符。
            // 举个例子：B -> b•AC，会把A在左边的产生式拉进来。
            // 然后由于A右边有C，意思是为了归约出A右边必须出现C，
            // 所以新项目的展望符就是C的FIRST集合。

            // ①右边没有新符号了
            if (curTerm.dotPos == curTerm.rightPart.size() - 1)
                newSubsequenceSet = curTerm.subsequence;
            // ② 如果•不在末尾(右边还有终结符)，则将该终结符(其FIRST集合即自身)拉入展望符中
            else if (VtToIndex.count(
                    curTerm.rightPart[curTerm.dotPos + 1]) != 0)
                newSubsequenceSet.push_back(curTerm.rightPart[curTerm.dotPos + 1]);
            // ③ 如果右边还有非终结符，把该非终结符的FIRST集合拉进来
            else {
                // 原文说的为啥是遍历FIRST里的终结符
                // 但是下面又是用的非终结符的映射啊？= =不懂了。
                newSubsequence = curTerm.rightPart[curTerm.dotPos + 1];
                int indexOfCurVn = VnToIndex[newSubsequence];
                set<string> FIRSTOfCurVn = FIRST[VnToIndex[newSubsequence]];
                for (const auto& vn : FIRST[VnToIndex[newSubsequence]])
                    newSubsequenceSet.push_back(vn);
            }
            newTerm.subsequence = newSubsequenceSet;
            // ATTENTION:       这里是后继符为空报错（说起来为空的话不是可以用M->null或者N->null来规约嘛？）
//            if (newTerm.subsequence.empty())
//                std::cerr << "后继符为空！" << endl;

            int newTermFlag = -1;
            // 如果和已有项目的左部/右部/•位置都相同，就合并
            // 为什么不用重载的==符号呢，因为那个还要求展望符也相同
            for (int k = 0; k < stateSet[stateIndex].size(); ++k) {
                if (newTerm.leftPart == stateSet[stateIndex][k].leftPart
                && newTerm.rightPart == stateSet[stateIndex][k].rightPart
                && newTerm.dotPos == stateSet[stateIndex][k].dotPos)
                    newTermFlag = k;
            }
            // 如果不在该状态的已有项目中，就作为新项目加入(否则不加入，只合并展望符)
            if (newTermFlag == -1) {
                termQueue.push(newTerm);
                stateSet[stateIndex].push_back(newTerm);
            } else {
                // 在将拉入的新项目与原来状态集的项目进行合并时进行检查。
                // ① 收集即将并入的项目的所有展望符
                map<string, int> subsequenceMap;
                for (auto & m : stateSet[stateIndex][newTermFlag].subsequence)
                    subsequenceMap[m]++;
                // ② 检查新项目的展望符不在即将并入的项目中，就加入
                for (auto & m : newTerm.subsequence) {
                    if (subsequenceMap[m] == 0)
                        stateSet[stateIndex][newTermFlag].subsequence.push_back(m);
                }
            }
        }
        // 该项目处理完毕
        termQueue.pop();
    }
}

int SemanticAnalyzer::GOTO(int stateIndex, const string& symbol) {
    int stateSize = stateSet[stateIndex].size();
    for (int i = 0; i < stateSize; ++i) {
        vector<string> rightPart = stateSet[stateIndex][i].rightPart;
        int dotPos = stateSet[stateIndex][i].dotPos;

        // ①•右边有(非)终结符 ②接受的符号正好是该项目•右边的符号
        if (dotPos < rightPart.size() && symbol == rightPart[dotPos]) {
            Term tmpTerm = stateSet[stateIndex][i];
            // 进行规约，•后移一位
            tmpTerm.dotPos += 1;
            dotPos = tmpTerm.dotPos;

            // 如果后移一位•到了最右，项目成为归约项目
            if (tmpTerm.dotPos == tmpTerm.rightPart.size()) {
                tmpTerm.type = REDUCE;
                // 新增项目集，该归约项目加入新项目集
                stateSet[globalStateCount].push_back(tmpTerm);
            } else if (VtToIndex.count(tmpTerm.rightPart[dotPos]) != 0) {
                // 如果•不在最右，且后面是终结符，则项目成为移进项目
                tmpTerm.type = SHIFT;
                stateSet[globalStateCount].push_back(tmpTerm);
            } else if (VnToIndex.count(tmpTerm.rightPart[dotPos]) != 0) {
                // 如果•不在最右，且后面是非终结符，则项目成为待约项目
                // 将该非终结符作为产生式左部的产生式拉入项目集中
                tmpTerm.type = TO_REDUCE;
                stateSet[globalStateCount].push_back(tmpTerm);
                // 计算该新项目集的闭包
                calculateClosure(globalStateCount);
            }
        }
    }

    // 先假设该状态为新状态
    globalStateCount++;
    // 计算该状态是否与前面的相同（可合并），flag记录相同的状态的索引
    int flag = mergeSet();
    // 如果flag=-1即为全新状态；否则进行合并，状态数-1
    if (flag != -1) {
        stateSet[globalStateCount - 1].clear();
        globalStateCount--;
        return flag;
    }
    else
        return globalStateCount - 1;
}

void SemanticAnalyzer::printStateSet() {
    for (int i = 0; i < globalStateCount; ++i) {
        if (stateSet[i].empty())
            continue;

        cout << "---------------------\n";
        cout << "State I" << i << "\n";
        for (const auto& term : stateSet[i]) {
            cout << '\t' << term.leftPart << " -> ";
            for (int j = 0; j < term.rightPart.size(); ++j) {
                if (j == term.dotPos)
                    cout << "•";
                cout << term.rightPart[j];
            }
            if (term.rightPart.size() == term.dotPos)
                cout << "•";
            for (int j = 0; j < term.subsequence.size(); ++j) {
                if (j == 0)
                    cout << "," << term.subsequence[j];
                else
                    cout << "/" << term.subsequence[j];
            }
            cout << "\t\n";
        }
        cout << "---------------------\n\n";
    }
}

void SemanticAnalyzer::printStateTable() {
    cout << "\t";
    for (const auto& vt : VtToIndex)
        cout << vt.first << "\t";
    for (const auto& vn : VnToIndex) {
        // 跳过增广文法的第一个产生式左部
        if (vn.first == GrammarAnalyzer::getVn(
                productionTable[0].substr(0, productionTable[0].length())))
            continue;
        cout << vn.first << "\t";
    }
    cout << '\n';
    for (int i = 0; i < globalStateCount; ++i) {
        if (stateSet[i].empty())
            continue;
        cout << i << '\t';
        // 构造ACTION表
        for (const auto& vt : VtToIndex) {
            int index = ActionTable[i][vt.second];
            if (isShiftTerm(index))
                cout << "s" << getShiftStateIndex(index) << '\t';
            else if (isReduceTerm(index))
                cout << "r" << getReduceStateIndex(index) << '\t';
            else if (isAcceptTerm(index))
                cout << "acc\t";
            else
                cout << index << "  \t";
        }

        // 构造GOTO表
        for (const auto& vn : VnToIndex) {
            if (vn.first == GrammarAnalyzer::getVn(
                    productionTable[0].substr(0, productionTable[0].length())))
                continue;
            cout << GotoTable[i][vn.second] << "  \t";
        }
        cout << endl;
    }
}

void SemanticAnalyzer::generateStateSet(int choice) {
    generateI0();
    calculateClosure(0);

    queue<string> symbolStack;
    map<string, int> symbolMap;

    int curState = 0;

    for (auto & term : stateSet[0]) {
        // 根据•右边的字判断是否能拉进来新的项目
        string symbol = term.rightPart[term.dotPos];
        if (symbolMap[symbol] == 0) {
            symbolStack.push(symbol);
            symbolMap[symbol]++;
        }
    }
    symbolStack.push("separator");

    while (!symbolStack.empty()) {
        string front = symbolStack.front();
        if (symbolStack.front() == "separator") {
            for (auto & term : stateSet[curState]) {

                // •在最右 => 规约项目，判断用哪个产生式进行规约
                if (term.dotPos == term.rightPart.size()) {

                    Term tmpTerm = term;
                    // 构造该产生式
                    string reduceTerm = tmpTerm.leftPart + "->";
                    for (auto & k : tmpTerm.rightPart)
                        reduceTerm += k;

                    int productionIndex = -1;
                    // 产生式相同 => 用该产生式进行归约(获取索引)
                    for (int k = 0; k < productionTable.size(); ++k) {
                        if (reduceTerm == productionTable[k])
                            productionIndex = k;
                    }

                    // 索引为0 => 用增光文法的第一个产生式进行归约 => 接受态
                    if (productionIndex == 0)
                        ActionTable[curState][VtToIndex["#"]] = ACCEPT_BASE;
                    else {

                        // LR(1)分析法 => 归约项后面出现展望符才进行规约
                        for (auto & k : tmpTerm.subsequence) {
                            // 不等于0 => 该位置已经被写入过了 => 有两个规约方式 => 规约-规约冲突
                            if (ActionTable[curState][VtToIndex[k]] != 0)
                                std::cerr << "(状态" << curState << "存在规约-规约冲突)" << endl;
                            ActionTable[curState][VtToIndex[k]] =
                                    REDUCE_BASE + productionIndex;
                        }
                    }
                }
            }

            // ACTION表规约项目构造完毕
            curState++;
            symbolStack.pop();
            continue;
        }

        int nextState = GOTO(curState, symbolStack.front());
        // ATTENTION:       标记：这里是打印了goto表内部的状态转移情况,所以注释掉
//        cout << "I" << curState << "--" << symbolStack.front()
//            << "-->" << "I" << nextState;

        // 填充ACTION表移进项目
        if (VtToIndex.count(symbolStack.front()) != 0) {
            // 如果已经填充过了
            if (ActionTable[curState][VtToIndex[symbolStack.front()]] != 0)
                std::cerr << "(状态" << curState << "移进" << symbolStack.front() << "存在冲突)";

            ActionTable[curState][VtToIndex[symbolStack.front()]] = SHIFT_BASE + nextState;
        } else
            GotoTable[curState][VnToIndex[symbolStack.front()]] = nextState;
//        cout << endl;

        if (nextState == globalStateCount - 1) {
            symbolMap.clear();

            for (auto & j : stateSet[nextState]) {

                // 如果是归约项目 => 跳过
                if (j.dotPos == j.rightPart.size())
                    continue;
                // 获取•右边的符号
                string symbol = j.rightPart[j.dotPos];
                if (symbolMap[symbol] == 0) {
                    symbolStack.push(symbol);
                    symbolMap[symbol]++;
                }
            }
            symbolStack.push("separator");
        }
        symbolStack.pop();

    }

//    printStateSet();
//    printStateTable();
//    printStateTableToFile("LR1_state_table.csv");
}

void SemanticAnalyzer::generateLALRTable() {
    cout << "Generating LALR parsing table\n";
    // 记录所有项目集是否已经被合并了
    bool merged[maxTermCount];
    // 初始化为全0
    std::fill(merged, merged + maxTermCount, false);

    // 记录合并进了哪些状态集
    map<int, int> mergedToWhichState;
    int flag = -1;

    for (int i = 0; i < globalStateCount; ++i) {
        // 如果该项目集已经被合并了
        if (merged[i])
            continue;

        for (int j = i + 1; j < globalStateCount; ++j) {
            if (merged[j])
                continue;

            // 项目集项目数不同也pass
            if (stateSet[i].size() != stateSet[j].size())
                continue;

            // 项目集数量相同，先假设可以合并
            flag = j;
            for (int k = 0; k < stateSet[j].size(); ++k) {
                if (stateSet[i][k].leftPart != stateSet[j][k].leftPart
                || stateSet[i][k].rightPart != stateSet[j][k].rightPart
                || stateSet[i][k].dotPos != stateSet[j][k].dotPos) {
                    flag = -1;
                    break;
                }
            }

            if (flag != -1) {
                // 是同心集，进行合并(合并展望符)

                for (int k = 0; k < stateSet[i].size(); ++k) {
                    map<string, int> subsequenceMap;

                    // 先记录被合并的项目的展望符
                    for (auto & sub : stateSet[i][k].subsequence)
                        subsequenceMap[sub]++;

                    // 若新并入的项目有新展望符，则进行合并
                    for (int n = 0; n < stateSet[j][k].subsequence.size(); ++n) {
                        if (subsequenceMap.count(stateSet[j][k].subsequence[n]) == 0)
                            stateSet[i][k].subsequence.push_back((stateSet[j][k].subsequence[n]));
                    }
                    subsequenceMap.clear();
                }

                // 将项目集j合并入项目集i
                mergedToWhichState[j] = i;

                // 合并后GOTO表不会有冲突，也不会产生新的移进-归约冲突
                // 如果有冲突说明文法不是LALR文法

                for (const auto& vt : VtToIndex) {
                    if (isReduceTerm(ActionTable[j][vt.second])) {
                        // 若被合并的状态集i未填充，则填充j的动作
                        if (ActionTable[i][vt.second] == 0)
                            ActionTable[i][vt.second] = ActionTable[j][vt.second];
                        // 如果都是归约项目，但是归约动作不同
                        else if (isReduceTerm(ActionTable[i][vt.second])
                        && ActionTable[i][vt.second] != ActionTable[j][vt.second])
                            std::cerr << "产生移进-归约冲突，文法不是LALR文法，请手动修改" << endl;
                    }
                }

                // 状态集J已并入状态集I，删除状态集J
                stateSet[j].clear();
                merged[j] = true;
            }
        }
    }

    // 处理移进项目
    for (int i = 0; i < globalStateCount; ++i) {
        if (merged[i])
            continue;

        for (const auto& vt : VtToIndex) {
            // 是移进项目且已经被合并
            if (isShiftTerm(ActionTable[i][vt.second])
            && merged[ActionTable[i][vt.second] - SHIFT_BASE])
                ActionTable[i][vt.second] =
                        mergedToWhichState[
                                ActionTable[i][vt.second] - SHIFT_BASE] + SHIFT_BASE;
        }

        for (const auto& vn : VnToIndex) {
            // 如果合并，就填入被合并进的状态的GOTO状态索引
            if (merged[GotoTable[i][vn.second]])
                GotoTable[i][vn.second] = mergedToWhichState[GotoTable[i][vn.second]];
        }
    }

//    printStateTable();

    // FIXME:不知道为什么ACTION表的else列有点问题……这里人为做了修正。

    ActionTable[6][VtToIndex["else"]] = REDUCE_BASE + 34;
    ActionTable[13][VtToIndex["else"]] = REDUCE_BASE + 17;
    ActionTable[21][VtToIndex["else"]] = REDUCE_BASE + 13;
    ActionTable[22][VtToIndex["else"]] = REDUCE_BASE + 23;
    ActionTable[24][VtToIndex["else"]] = REDUCE_BASE + 43;
    ActionTable[25][VtToIndex["else"]] = REDUCE_BASE + 15;
    ActionTable[43][VtToIndex["else"]] = REDUCE_BASE + 39;
    ActionTable[109][VtToIndex["else"]] = REDUCE_BASE + 1;
    ActionTable[135][VtToIndex["else"]] = REDUCE_BASE + 14;

//    ActionTable[379][VtToIndex["goto"]] = REDUCE_BASE + 41;
//    ActionTable[379][VtToIndex["null"]] = ERROR;


//    ActionTable[13][VtToIndex["goto"]] = REDUCE_BASE + 17;
//    ActionTable[21][VtToIndex["goto"]] = REDUCE_BASE + 13;
//    ActionTable[22][VtToIndex["goto"]] = REDUCE_BASE + 23;
//    ActionTable[24][VtToIndex["goto"]] = REDUCE_BASE + 43;
//    ActionTable[25][VtToIndex["goto"]] = REDUCE_BASE + 15;
//    ActionTable[78][VtToIndex["goto"]] = REDUCE_BASE + 24;
//    ActionTable[127][VtToIndex["goto"]] = REDUCE_BASE + 4;
//    ActionTable[135][VtToIndex["goto"]] = REDUCE_BASE + 14;
//    ActionTable[145][VtToIndex["goto"]] = REDUCE_BASE + 16;
//    ActionTable[152][VtToIndex["goto"]] = REDUCE_BASE + 44;


//    GotoTable[0][VnToIndex["S"]] = 28;
//    GotoTable[0][VnToIndex["L"]] = 27;
//    ActionTable[246][VtToIndex["#"]] = REDUCE_BASE + 18;
    printStateTableToFile("amended_LALR_state_table.csv");
}

//void SemanticAnalyzer::readStateTable(const string& fileName) {
//    std::ifstream inFile(fileName, std::ios::in);
//    string line;
//    vector<vector<int>> GotoTable1, ActionTable1;
//    while(std::getline(inFile, line)) {
//        cout << line << endl;
//    }
//}

void SemanticAnalyzer::printStateTableToFile(const string& fileName) {

    std::ofstream outFile;
    outFile.open(fileName, std::ios::out);

    outFile << ",";
    for(const auto& vt: VtToIndex) {
        if (vt.first == ",")
            outFile << "\",\",";
        else
            outFile << vt.first << ",";
    }
    for (const auto& vn :VnToIndex) {
        if (vn.first == GrammarAnalyzer::getVn(
                productionTable[0].substr(0, productionTable[0].length())))
            continue;
        outFile << vn.first << ",";
    }
    outFile << "\n";
    for (int i = 0; i < globalStateCount; ++i) {
        if (stateSet[i].empty())
            continue;
        outFile << i << ",";
        for (const auto& vt : VtToIndex) {
            int index = ActionTable[i][vt.second];
            if (isShiftTerm(index))
                outFile << "s" << getShiftStateIndex(index) << ",";
            else if (isReduceTerm(index))
                outFile << "r" << getReduceStateIndex(index) << ",";
            else if (isAcceptTerm(index))
                outFile << "acc,";
            else
                outFile << index << ",";
        }

        for (const auto& vn : VnToIndex) {
            if (vn.first == GrammarAnalyzer::getVn(
                    productionTable[0].substr(0, productionTable[0].length())))
                continue;
            outFile << GotoTable[i][vn.second] << ",";
        }
        outFile << "\n";
    }

    outFile.close();
}

void SemanticAnalyzer::printStateTableToFilePureNumber(const string &fileName) {
    std::ofstream outFile;
    outFile.open(fileName, std::ios::out);

    for (int i = 0; i < maxTermCount; ++i) {
        for (int item : ActionTable[i])
            outFile << item << ",";
        for (int item : GotoTable[i])
            outFile << item << ",";
        outFile << "\n";
    }
    outFile.close();
}

void SemanticAnalyzer::readStateTable(const string &fileName) {
    std::ifstream inFile(fileName, std::ios::in);
    string line;
    vector<vector<int>> tableTemp;
    while(std::getline(inFile, line)) {
        std::stringstream lineStr(line);
        string number;
        vector<int> rowTemp;
        while(std::getline(lineStr, number, ',')) {
            rowTemp.push_back(std::atoi(number.c_str()));
        }
        tableTemp.push_back(rowTemp);
    }
    cout << "tablesize: " << tableTemp.size() << " * " << tableTemp[0].size();
    for (int i = 0; i < maxTermCount; ++i) {
        for (int j = 0; j < 60; ++j)
            ActionTable[i][j] = tableTemp[i][j];
        for (int j = 60; j < 120; ++j)
            GotoTable[i][j] = tableTemp[i][j];
    }
}


#endif //INTERMEDIATE_CODE_GENERATOR_SEMANTICANALYZER_H
