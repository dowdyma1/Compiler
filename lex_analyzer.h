#ifndef __LEX_ANALYZER_H
#define __LEX_ANALYZER_H

#include <stdbool.h>

typedef enum{
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym,
    slashsym, oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym,
    lparentsym, rparentsym, commasym, semicolonsym, periodsym, 
    becomessym, beginsym, endsym, ifsym, thensym, whilesym, dosym, 
    callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;


struct entry{
    char lexeme[10]; // character representation of the token
    int tokenType; // integer value of the token
    struct entry* next;
};

struct entry* doLex(char *filename, bool lflag);

void printTable(struct entry* tableHead);


#endif