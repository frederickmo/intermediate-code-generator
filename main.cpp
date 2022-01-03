
//#include "SemanticAnalyzer.h"

#include "QuadrupleTranslator.h"

#include <iostream>



int main() {

    GrammarAnalyzer grammarAnalyzer;
    grammarAnalyzer.scanProduction();
    grammarAnalyzer.calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer semanticAnalyzer;

    semanticAnalyzer.generateStateSet();
    semanticAnalyzer.generateLALRTable();
//    semanticAnalyzer.printStateTableToFilePureNumber("state_table_pure_number.csv");

//    semanticAnalyzer.readStateTable("state_table_pure_number.csv");
//
    QuadrupleTranslator quadrupleTranslator;
    quadrupleTranslator.lex();
    quadrupleTranslator.parse();

    for (const auto& quadruple : newQuadrupleList)
        quadruple.print();



    return 0;
}