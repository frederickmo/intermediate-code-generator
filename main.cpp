
#include "QuadrupleGenerator.h"

int main() {

    GrammarAnalyzer::scanProduction();
    GrammarAnalyzer::calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer::generateStateSet();
    SemanticAnalyzer::generateLALRTable();

    QuadrupleGenerator::lex(false);
    QuadrupleGenerator::parse(false);
    QuadrupleGenerator::printQuadruples();
    QuadrupleGenerator::printResultToFile();

    return 0;
}