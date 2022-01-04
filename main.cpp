
#include "QuadrupleTranslator.h"

int main() {

    GrammarAnalyzer::scanProduction();
    GrammarAnalyzer::calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer::generateStateSet();
    SemanticAnalyzer::generateLALRTable();

    QuadrupleTranslator::lex();
    QuadrupleTranslator::parse();

    for (const auto& quadruple : newQuadrupleList) {
        quadruple.printAsQuadruple();
    }
    cout << endl;

    for (const auto& quadruple : newQuadrupleList) {
        quadruple.printArgInName();
    }


    return 0;
}