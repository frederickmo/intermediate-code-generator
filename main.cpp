
#include "QuadrupleGenerator.h"

int main() {

    GrammarAnalyzer::scanProduction();
    GrammarAnalyzer::calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer::generateStateSet();
    SemanticAnalyzer::generateLALRTable();

<<<<<<< Updated upstream
    QuadrupleTranslator::lex();
    QuadrupleTranslator::parse();

    cout << "四元式形式\n";
    int address = offset;
    for (const auto& quadruple : newQuadrupleList) {
        cout << "(" << address << ") ";
        quadruple.print();
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

=======
    QuadrupleGenerator::lex(false);
    QuadrupleGenerator::parse(false);
    QuadrupleGenerator::printQuadruples();
>>>>>>> Stashed changes

    return 0;
}