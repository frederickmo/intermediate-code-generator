
//#include "SemanticAnalyzer.h"

#include "QuadrupleTranslator.h"

#include <iostream>



int main() {

    productionTable = {
//            "E -> E1 or M E2",
//            "E -> E1 and M E2",
//            "E -> not E1",
//            "E -> (E1)",
//            "E -> i relop j",
//            "E -> i",
//            "E -> j",
//            "M -> null"

//            "S1 -> S",
//            "S -> if E then M S",
//            "S -> if E then M S N else M S",
//            "S -> while M E do M S",
//            "E -> E or M E",
//            "E -> E and M E",
//            "E -> not E",
//            "E -> (E)",
//            "E -> i relop i",
//            "E -> i",
//            "M -> null",
//            "N -> null"

//            "S->A",
//            "A->BB",
//            "B->aB",
//            "B->b"

//            "A->S",
//            "S->E",
//            "E->E+T",
//            "E->T",
//            "T->T*F",
//            "T->F",
//            "F->(E)",
//            "F->i"





//            "S1->S",
//            "S->E",
//            "E->EorMT",
//            "E->T",
//            "T->TandMF",
//            "T->F",
//            "F->(E)",
//            "F->notF",
//            "M->null",
//            "F->Q<Q",
//            "F->Q<=Q",
//            "F->Q==Q",
//            "F->Q!=Q",
//            "F->Q>Q",
//            "F->Q>=Q",
//            "F->i<i",
//            "F->i<=i",
//            "F->i==i",
//            "F->i!=i",
//            "F->i>i",
//            "F->i>=i",
//            "F->i"



/*0*/            "<<ACC>>-><<START>>",

/*1*/            "A-><<ID>>:=<<EXPR>>",

/*2*/            "<<BAND>>-><<BNOT>>",
/*3*/            "<<BAND>>-><<BAND>>andM<<BNOT>>",

/*4*/            "<<BCMP>>-><<EXPR>><<RELOP>><<EXPR>>",
/*5*/            "<<BCMP>>-><<EXPR>>",
/*6*/            "<<BCMP>>->(<<BEXPR>>)",

/*7*/            "<<BEXPR>>-><<BAND>>",
/*8*/            "<<BEXPR>>-><<BEXPR>>orM<<BAND>>",

/*9*/            "<<BNOT>>-><<BCMP>>",
/*10*/            "<<BNOT>>->not<<BNOT>>",

/*11*/            "<<ELIST>>-><<EXPR>>",
/*12*/            "<<ELIST>>-><<ELIST>>,<<EXPR>>",

/*13*/            "<<EXPR>>-><<NEG>>",
/*14*/            "<<EXPR>>-><<EXPR>>+<<TERM>>",

/*15*/            "<<FACTOR>>-><<ID>>",
/*16*/            "<<FACTOR>>->(<<EXPR>>)",

/*17*/            "<<ID>>->i", // variable_name

/*18*/            "L->L;MS",
/*19*/            "L->S",

/*20*/            "<<LABEL>>-><<ID>>:",

/*21*/            "M->null",
/*22*/            "N->null",

/*23*/            "<<NEG>>-><<TERM>>",
/*24*/            "<<NEG>>->-<<TERM>>",

/*25*/            "<<OPENSTMT>>->if<<BEXPR>>thenM<<STMT>>",
/*26*/            "<<OPENSTMT>>->if<<BEXPR>>thenMSNelseM<<OPENSTMT>>",

/*27*/            "<<RELOP>>->=",
/*28*/            "<<RELOP>>->!=",
/*29*/            "<<RELOP>>-><",
/*30*/            "<<RELOP>>-><=",
/*31*/            "<<RELOP>>->>",
/*32*/            "<<RELOP>>->>=",

/*33*/            "S->if<<BEXPR>>thenMSNelseMS",
/*34*/            "S->A",
/*35*/            "S->{L}",
/*36*/            "S->whileM<<BEXPR>>doMS",
/*37*/            "S->call<<ID>>(<<ELIST>>)",
/*38*/            "S->labelS",
/*39*/            "S->goto<<ID>>",

/*40*/            "<<START>>-><<STMT>>",

/*41*/            "<<STMT>>->S",
/*42*/            "<<STMT>>-><<OPENSTMT>>",

/*43*/            "<<TERM>>-><<FACTOR>>",
/*44*/            "<<TERM>>-><<TERM>>*<<FACTOR>>"



    };

//    GrammarAnalyzer grammarAnalyzer;
//    grammarAnalyzer.setProductionTable(productionTable);
//    grammarAnalyzer.scanProduction();
//    grammarAnalyzer.calculateFinalFIRSTAndFOLLOWSets();

    GrammarAnalyzer grammarAnalyzer;
    grammarAnalyzer.scanProduction();
    grammarAnalyzer.calculateFinalFIRSTAndFOLLOWSets();

    SemanticAnalyzer semanticAnalyzer;

//    semanticAnalyzer.productionTable = productionTable;
    semanticAnalyzer.generateStateSet();
    semanticAnalyzer.generateLALRTable();

    QuadrupleTranslator quadrupleTranslator;
    quadrupleTranslator.lex();
    quadrupleTranslator.parse();

    for (const auto& quadruple : quadrupleOrTAC)
        cout << quadruple;



    return 0;
}