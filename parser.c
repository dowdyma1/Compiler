/*
 * Matthew Dowdy
 * 12/04/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lex_analyzer.h"
#include "parser.h"


void setHead(struct entry *list);
void getToken();
void addSymbol(int kind, char name[10], int level, int val, int addr, bool isMarked);
int checkTable(char name[10]);
void program();
void block(int lexlevel);
int const_declaration(int lexlevel);
int var_declaration(int lexlevel);
int procedure_declaration(int lexlevel);
void statement(int lexlevel);
void condition(int lexlevel);
void expression(int lexlevel);
void term(int lexlevel);
void factor(int lexlevel);
int isHalt();
int tableSize();
symbol * parse(struct entry *list);

char tokenName[10];
int tp = 0;

int isError = 0;

int token;
symbol *symbol_table = NULL;

struct entry *cur_token = NULL;

int isHalt(){
    return isError;
}

void setHead(struct entry *list){
    cur_token = list;
}

void getToken(){
    if(cur_token != NULL){
        token = cur_token->tokenType;
        strcpy(tokenName, cur_token->lexeme);
    }
    else{ // error, no more tokens
        printf("ERROR: UH OH, no more tokens\n");
        token = 0;
    }

    cur_token = cur_token->next;

}

int tableSize(){
    return tp;
}

void genTable(){
    symbol_table = malloc(MAX_SYMBOL_TABLE_SIZE * sizeof(symbol));
}


void errorFn(int num){
    isError = 1;
    printf("\nError (%d). ", num);
    switch(num){
        case 1:
            printf("Use = instead of :=");
            break;
        case 2:
            printf("= must be followed by a number");
            break;
        case 3:
            printf("Identifier must be followed by =");
            break;
        case 4:
            printf("const, var, procedure must be followed by identifier");
            break;
        case 5:
            printf("Semicolon or comma missing");
            break;
        case 6:
            printf("Incorrect symbol after procedure declaration");
            break;
        case 7:
            printf("Statement expected");
            break;
        case 8:
            printf("Incorrect symbol after statement part in block");
            break;
        case 9:
            printf("Period expected");
            break;
        case 10:
            printf("Semicolon between statements missing");
            break;
        case 11:
            printf("Undeclared identifier");
            break;
        case 12:
            printf("Assignment to constant or procedure not allowed");
            break;
        case 13:
            printf("Assignment operator expected");
            break;
        case 14:
            printf("call must be followed by an identifer");
            break;
        case 15:
            printf("Call of a constant or variable is meaningless");
            break;
        case 16:
            printf("then expected");
            break;
        case 17:
            printf("Semicolon or } expected");
            break;
        case 18:
            printf("do expected");
            break;
        case 19:
            printf("Incorrect symbol following statement");
            break;
        case 20:
            printf("Relational operator expected");
            break;
        case 21:
            printf("Expression must not containt a procedure identifier");
            break;
        case 22:
            printf("Right parenthesis missing");
            break;
        case 23:
            printf("The preceding factor cannot begin with this symbol");
            break;
        case 24:
            printf("An expression cannot begin with this symbol");
            break;
        case 25:
            printf("This number is too large");
            break;
        case 26:
            printf("Variable already initialized");
            break;
        case 27:
            printf("end expected");
            break;
        case 28:
            printf("identifier expeceted after read or write");
            break;
        case 29:
            printf(
                "Procedure not initialized, or procedure already marked, or wrong lex level");
            break;
        case 30:
            printf("Identifier inexcessible");
            break;
        case 31:
            printf("Procedure is inexcessible");
            break;
        case 32:
            printf("Identifier cannot be read");
            break;
    }
    printf(".\n\n");
    return;
}

void addSymbol(int kind, char name[10], int level, int val, int addr, bool isMarked){
    symbol_table[tp].kind = kind;
    strcpy(symbol_table[tp].name, name);
    symbol_table[tp].val = val;
    symbol_table[tp].level = level;
    symbol_table[tp].addr = addr;
    symbol_table[tp].isMarked = isMarked;

    tp++;
}

// gets the position of the desired row in the table
int checkTable(char name[10]){
    for(int i = 0; i < tp; i++){
        if(strcmp(symbol_table[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}

void program(){
    if(isError){
        return;
    }

    addSymbol( 3, "main", 0, 0, 0, false);

    getToken();
    block(0);
    if(isError){return;}

    if(token != periodsym){
        errorFn(9);
    }
}

void block(int lexlevel){
    if(isError){
        return;
    }

    int numSymbols = 0;

    numSymbols += const_declaration(lexlevel);
    numSymbols += var_declaration(lexlevel);
    //numSymbols += procedure_declaration(lexlevel + 1);
    numSymbols += procedure_declaration(lexlevel);

    statement(lexlevel);

    // ^ mark the last numSymbols number of unmarked symbols
    int i = 0;
    while(numSymbols > 0){
        if(tp-1-i < 0){printf("ERROR\n");break;}

        if(!symbol_table[tp-1-i].isMarked){
            symbol_table[tp-1-i].isMarked = true;
            numSymbols--;
        }
        i++;
    }
}

int const_declaration(int lexlevel){
    int numConsts = 0;
    // CONST-DECLARATION
    if(token == constsym){
        do{
            getToken();
            
            if(token != identsym){errorFn(4); return numConsts;}

            int tablePos = checkTable(tokenName);
            // ident name in sTable, unmarked & at same lex
            if(tablePos >= 0 && symbol_table[tablePos].isMarked == false && 
                symbol_table[tablePos].level == lexlevel){
                errorFn(12); return numConsts;}

            char tempName[10];
            strcpy(tempName, tokenName);

            getToken();

            if(token != eqsym){errorFn(3); return numConsts;}
            getToken();

            if(token != numbersym){errorFn(24); return numConsts;}

            addSymbol(1, tempName, lexlevel, atoi(tokenName), 0, false);
            getToken();

            numConsts++;

        }while(token == commasym);

        if(token != semicolonsym){errorFn(17); return numConsts;}
        getToken();
    }

    return numConsts;
}

int var_declaration(int lexlevel){
    // VAR-DECLARATION
    int numVars = 0;
    if(token == varsym){
        do{
            getToken();

            if(token != identsym){errorFn(4); return numVars;}

            int tablePos = checkTable(tokenName);
            if(tablePos >= 0 && symbol_table[tablePos].isMarked == false && 
                symbol_table[tablePos].level == lexlevel){
                errorFn(26); return numVars;}

            numVars++;
            addSymbol(2, tokenName, lexlevel, 0, numVars + 2, false);
            getToken();
        }while(token == commasym);


        if(token != semicolonsym){errorFn(17); return numVars;}
        getToken();
    }

    return numVars;
}

int procedure_declaration(int lexlevel){
    int numProcs = 0;

    if(token == procsym){
        do{
            numProcs++;
            getToken();

            if(token != identsym){errorFn(4); return numProcs;}


            int tablePos = checkTable(tokenName);
            if(tablePos >= 0 && symbol_table[tablePos].isMarked == false && 
                symbol_table[tablePos].level == lexlevel){
                errorFn(29); return numProcs;}

            addSymbol(3, tokenName, lexlevel, 0, 0, false);

            getToken();

            if(token != semicolonsym){errorFn(5); return numProcs;}

            getToken();
            block(lexlevel+1);

            if(token != semicolonsym){errorFn(5); return numProcs;}

            getToken();
        }while(token == procsym);

    }

    return numProcs;
}

void statement(int lexlevel){
    if(isError){return;}

    int symLoc;
    bool found;
    if(token == identsym){
        found = false;

        // look for this identifier as unmarked var in sym table
        for(int i = tp-1; i >=0; i--){
            if(!symbol_table[i].isMarked && symbol_table[i].kind == 2 &&
                    strcmp(tokenName, symbol_table[i].name) == 0){
                found = true;
                break;
            }
        }

        if(!found){errorFn(30); return;}

        getToken();

        if(token != becomessym){errorFn(13); return;}
        getToken();

        expression(lexlevel);
        if(isError){return;}
    }
    if(token == callsym){
        found = false;
        getToken();

        for(int i = tp-1; i >=0; i--){
            if(!symbol_table[i].isMarked && symbol_table[i].kind == 3 &&
                    strcmp(tokenName, symbol_table[i].name) == 0){
                found = true;
                break;
            }
        }

        if(!found){errorFn(31); return;}

        getToken();
    }

    if(token == beginsym){
        getToken();
        statement(lexlevel);
        if(isError){return;}

        while(token == semicolonsym){
            getToken();
            statement(lexlevel);
            if(isError){return;}
        }

        if(token != endsym){errorFn(27); return;}
        getToken();
    }

    if(token == ifsym){
        getToken();
        condition(lexlevel);
        if(isError){return;}

        if(token != thensym){errorFn(16); return;}
        getToken();
        statement(lexlevel);

        if(token == elsesym){
            getToken();
            statement(lexlevel);
        }

        if(isError){return;}

    }

    if(token == whilesym){
        getToken();
        condition(lexlevel);
        if(isError){return;}

        if(token != dosym){errorFn(18); return;}
        getToken();
        statement(lexlevel);
        if(isError){return;}
    }

    if(token == readsym){
        getToken();
        
        found = false;

        for(int i = tp-1; i >=0; i--){
            if(!symbol_table[i].isMarked && symbol_table[i].kind == 2 &&
                    strcmp(tokenName, symbol_table[i].name) == 0){
                found = true;
                break;
            }
        }

        if(!found){errorFn(32); return;}

        getToken();
    }

    if(token == writesym){
        getToken();
        expression(lexlevel);
    }
}


void condition(int lexlevel){
    if(isError){
        return;
    }

    if(token == oddsym){
        getToken();
        expression(lexlevel);
        if(isError){return;}
    }
    else{
        expression(lexlevel);
        if(isError){return;}
        if(token != eqsym && token != neqsym && token != lessym && 
                token != leqsym && token != gtrsym && token != geqsym){
            errorFn(20); return;}
        getToken();
        expression(lexlevel);
        if(isError){return;}
    }
}

void expression(int lexlevel){
    if(isError){
        return;
    }

    if(token == plussym || token == minussym){
        getToken();
    }
    term(lexlevel);
    if(isError){return;}

    while(token == plussym || token == minussym){
        getToken();
        term(lexlevel);
        if(isError){return;}
    }
}

void term(int lexlevel){
    if(isError){
        return;
    }

    factor(lexlevel);
    if(isError){return;}

    while(token == multsym || token == slashsym){
        getToken();
        factor(lexlevel);
        if(isError){return;}
    }
}

void factor(int lexlevel){
    if(isError){
        return;
    }
    int symLoc;
    bool found;
    if(token == identsym){
        found = false;
        for(int i = tp-1; i >=0; i--){            
            if(!symbol_table[i].isMarked && 
                    (symbol_table[i].kind == 2 || 
                     symbol_table[i].kind == 1) &&
                    strcmp(tokenName, symbol_table[i].name) == 0){
                found = true;
                break;
            }
        }

        if(!found){errorFn(30); return;}

        getToken();
    }
    else if(token == numbersym){
        getToken();
    }
    else if(token == lparentsym){
        getToken();
        expression(lexlevel);
        if(isError){return;}
        if(token != rparentsym){errorFn(22); return;}
        getToken();
    }
    else{errorFn(-1); return;}
}


void printSymbolTable(){
    printf("\nkind\tname\tlevel\tval\taddr\tisMarked\n");

    for(int i = 0; i < tp; i++){
        printf("%d\t%s\t%d\t%d\t%d\t%d\n", symbol_table[i].kind,
                symbol_table[i].name, symbol_table[i].level,
                symbol_table[i].val, symbol_table[i].addr,
                symbol_table[i].isMarked);
    }

    printf("\n");
}


// returns symbol table
symbol * parse(struct entry *list){
    setHead(list);
    genTable();

    program();

    return symbol_table;
}