
#include "QuadrupleTranslator.h"

int main() {

    GrammarAnalyzer::scanProduction();
    GrammarAnalyzer::calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer::generateStateSet();
    SemanticAnalyzer::generateLALRTable();

    QuadrupleTranslator::lex();
    QuadrupleTranslator::parse();

    cout << "四元式形式\n";
    int address = offset;
    for (const auto& quadruple : newQuadrupleList) {
        cout << "(" << address << ") ";
        quadruple.printAsQuadruple();
        ++address;
    }
    cout << endl;
    address = offset;

    cout << "三地址代码形式\n";
    for (const auto& quadruple : newQuadrupleList) {
        cout << "(" << address << ") ";
        quadruple.printArgInName();
        ++address;
    }


    return 0;
}