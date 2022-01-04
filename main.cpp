
#include "QuadrupleTranslator.h"

int main() {

    GrammarAnalyzer::scanProduction();
    GrammarAnalyzer::calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer::generateStateSet();
    SemanticAnalyzer::generateLALRTable();

    QuadrupleTranslator::lex();
    QuadrupleTranslator::parse();

    cout << "四元式形式\n";
    for (const auto& quadruple : newQuadrupleList) {
        quadruple.printAsQuadruple();
    }
    cout << endl;

    cout << "三地址代码形式\n";
    for (const auto& quadruple : newQuadrupleList) {
        quadruple.printArgInName();
    }


    return 0;
}