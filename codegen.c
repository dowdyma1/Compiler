#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parser.h"
#include "lex_analyzer.h"
#include "codegen.h"
#define CODE_SIZE 1000
#define PRINTOUT_CODE 0
#define PRINTOUT_SYM_TABLE 0

void emit(int op, int r, int l, int m, char const *func);
void gErrorFn(int num);
void gProgram();
void gBlock(int lexlevel, char procName[10]);
void gStatement(int lexlevel);
void gCondition(int lexlevel);
void gExpression(int reg, int lexlevel);
void gTerm(int reg, int lexlevel);
void gFactor(int reg, int lexlevel);
char* opcode_to_str(int opcode);
int getCodeSize();
instruction *generate_code(symbol *table, struct entry *list, int *tableSize);

int funcCounter = 0;

char gTokenName[10];
int cx = 0;
int gToken, ts;
int gNumSymbols;
symbol *gSymbol_table = NULL;
struct entry *gCur_token = NULL;

instruction *code = NULL;

char* gOpcode_to_str(int opcode) {
    if(opcode == 1){return "lit";}
    if(opcode == 2){return "rtn";}
    if(opcode == 3){return "lod";}
    if(opcode == 4){return "sto";}
    if(opcode == 5){return "cal";}
    if(opcode == 6){return "inc";}
    if(opcode == 7){return "jmp";}
    if(opcode == 8){return "jpc";}
    if(opcode == 9){return "sys";}
    if(opcode == 10){return "neg";}
    if(opcode == 11){return "add";}
    if(opcode == 12){return "sub";}
    if(opcode == 13){return "mul";}
    if(opcode == 14){return "div";}
    if(opcode == 15){return "odd";}
    if(opcode == 16){return "mod";}
    if(opcode == 17){return "eql";}
    if(opcode == 18){return "neq";}
    if(opcode == 19){return "lss";}
    if(opcode == 20){return "leq";}
    if(opcode == 21){return "gtr";}
    if(opcode == 22){return "geq";}
}


void gSetHead(struct entry *list){
    gCur_token = list;
}

int getCodeSize(){
    return cx;
}

void gPrintSymbolTable(){
    printf("\nkind\tname\tlevel\tval\taddr\tisMarked\n");

    for(int i = 0; i < ts; i++){
        printf("%d\t%s\t%d\t%d\t%d\t%d\n", gSymbol_table[i].kind,
                gSymbol_table[i].name, gSymbol_table[i].level,
                gSymbol_table[i].val, gSymbol_table[i].addr,
                gSymbol_table[i].isMarked);
    }

    printf("\n");
}


void gGetToken(int times){
    while(times > 0){

        if(gCur_token != NULL){
            gToken = gCur_token->tokenType;
            strcpy(gTokenName, gCur_token->lexeme);

        }
        else{
            printf("uh oh..\n");
            gToken = 0;
            break;
        }
        gCur_token = gCur_token->next;
        
        times--;
    }
}

void gGenTable(symbol *table){
    gSymbol_table = malloc(MAX_SYMBOL_TABLE_SIZE * sizeof(symbol));

    for(int i = 0; i < ts; i++){
        gSymbol_table[i].kind = table[i].kind;
        strcpy(gSymbol_table[i].name, table[i].name);
        gSymbol_table[i].val = table[i].val;
        gSymbol_table[i].level = table[i].level;
        gSymbol_table[i].addr = table[i].addr;
        gSymbol_table[i].isMarked = table[i].isMarked;
    }
    
}

int gCheckTable(char name[10]){
    for(int i = 0; i < ts; i++){
        if(strcmp(gSymbol_table[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}

int gCheckTablev2(char name[10], int level){
    for(int i = ts - 1; i >= 0; i--){
        if(gSymbol_table[i].level <= level &&
            strcmp(gSymbol_table[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}

int gCheckTablev3(char name[10], int level){
    for(int i = ts - 1; i >= 0; i--){
        if(gSymbol_table[i].level <= level &&
            gSymbol_table[i].isMarked == false &&
            strcmp(gTokenName, gSymbol_table[i].name) == 0){
            return i;
        }
    }
    return -1;
}

void printCode(){
        printf("\top\tr\tl\tm\n");
    for(int i = 0; i < cx; i++){
        printf("%d\t%s\t%d\t%d\t%d\n",i, gOpcode_to_str(code[i].opcode), code[i].r,
                code[i].l, code[i].m);
    }
}


void emit (int op, int r, int l, int m, char const* func){
    if(cx > CODE_SIZE){
        // too many codes
        gErrorFn(1);
    }
    else{
        code[cx].opcode = op;
        code[cx].r = r;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }

    if(cx < 60){
        if(PRINTOUT_CODE){
            printf("current function: ");

            for(int i = 1; func[i] != '\0'; i++){
                printf("%c", func[i]);
            }

            printf("    current token: %s", gTokenName);

            printf("\n");

            printf("\tAssembly Instructions\n");
            printCode();

            for(int i = 0; i < 50; i++){
                printf("\n");
            }
        }

        if(PRINTOUT_SYM_TABLE){
            printf("current token: %s\n\n", gTokenName);
            printf("\t\tSymbol Table");
            gPrintSymbolTable();

            for(int i = 0; i < 50; i++){
                printf("\n");
            }
        }
    }
}


void gErrorFn(int num){
    printf("ERROR %d\n", num);
    return;
}

void gProgram(){
    int count = 1;
    for(int i = 0; i < ts; i++){
        if(gSymbol_table[i].kind == 3){
            gSymbol_table[i].val = count;
            count++;
            emit(7, 0, 0, 0, __func__); // JMP
        }
    }

    gGetToken(1);
    gBlock(0, "main");

    // fix the JMPs at the start
    int i = 0, startPos = 0;
    while(code[i].opcode == 7){
        for(int j = startPos; j < ts; j++){
            if(gSymbol_table[j].kind == 3){
                code[i].m = gSymbol_table[j].addr;
                startPos = j+1;
                break;
            }
        }
        i++;
    }

    int mVal, symAddr;
    for(i = 0; i < cx; i++){
        if(code[i].opcode == 5){ // CAL
            mVal = code[i].m;
            for(int j = 0; j < ts; j++){
                symAddr = gSymbol_table[j].addr;
                if(gSymbol_table[j].kind == 3 && symAddr  == mVal){
                    code[i].m = symAddr;
                    break;
                }
            }        
        }    
    }

    emit(9, 0, 0, 3, __func__); // HALT
}

void gBlock(int lexlevel, char procName[10]){
    gNumSymbols = 0;
    int gNumVars = 0;
    int tPos;

    if(gToken == constsym){
        do{
            gGetToken(1);
            gNumSymbols++;
            
            // unmarking the const in the symbol table
            tPos = gCheckTablev2(gTokenName, lexlevel);
            gSymbol_table[tPos].isMarked = false;

            gGetToken(3);

            //emit(1, 0, 0, atoi(gTokenName)); // LIT
            
        }while(gToken == commasym);
        gGetToken(1);
    }
    if(gToken == varsym){
        do{
            gGetToken(1);
            gNumVars++;
            gNumSymbols++;

            // unmarking the var in the symbol table
            tPos = gCheckTablev2(gTokenName, lexlevel);
            gSymbol_table[tPos].isMarked = false;

            gGetToken(1);
        }while(gToken == commasym);
        gGetToken(1);
    }

    if(gToken == procsym){
        do{
            gGetToken(1);
            gNumSymbols++;

            char tempName[10];
            strcpy(tempName, gTokenName);
            
            // unmarking the procedure in the symbol table
            tPos = gCheckTablev2(gTokenName, lexlevel);
            gSymbol_table[tPos].isMarked = false;

            gGetToken(2);
            gBlock(lexlevel + 1, tempName);
            emit(2, 0, 0, 0, __func__); // RTN
            gGetToken(1);
        }while(gToken == procsym);
    }

    int procInd = 0;
    for(int i = 0; i < ts; i++){
        if(gSymbol_table[i].level <= lexlevel &&
            gSymbol_table[i].kind == 3 &&
            strcmp(gSymbol_table[i].name, procName) == 0){
            procInd = i;
            break;
        }
     }
    // ^update sym table for this procedure (cur procedure) M = current code index
    gSymbol_table[procInd].addr = cx;

    emit(6, 0, 0, 3 + gNumVars, __func__); // INC

    gStatement(lexlevel);

    // ^ mark the last numSymbols number of unmarked symbols
    int i = 0, index;
    while(gNumSymbols > 0){
        index = ts-1-i;
        if(index < 0){printf("ERROR\n");break;}

        if(!gSymbol_table[index].isMarked){
            gSymbol_table[index].isMarked = true;
            gNumSymbols--;
        }
        i++;
    }
}

void gStatement(int lexlevel){
    int tLoc, ctemp, ctemp2;
    if(gToken == identsym){
        // ^ save the sym table index of the var entry unmarked and 
        // closest in lex level
        // fixIndex
        
        int fixIndex;

        fixIndex = gCheckTablev3(gTokenName, lexlevel);

        gGetToken(2);
        gExpression(0, lexlevel);

        emit(4, 0, lexlevel - gSymbol_table[fixIndex].level,
                gSymbol_table[fixIndex].addr, __func__); // STO

    }
    if(gToken == callsym){
        gGetToken(1); // procedure name

        // ^ save the sym table index of the procedure entry unmarked &
        // closest in lexlevel
        int procIndex = 0;
        
        procIndex = gCheckTablev3(gTokenName, lexlevel);

        emit(5, 0, lexlevel - gSymbol_table[procIndex].level,
                gSymbol_table[procIndex].addr, __func__); // CAL

        gGetToken(1);
    }

    if(gToken == beginsym){
        gGetToken(1);
        gStatement(lexlevel);
        while(gToken == semicolonsym){
            gGetToken(1);
            gStatement(lexlevel); // 47
        }
        gGetToken(1);
    }

    if(gToken == ifsym){
        gGetToken(1);
        gCondition(lexlevel);
        ctemp = cx;
        emit(8, 0, 0, 0, __func__); // JPC
        gGetToken(1);
        gStatement(lexlevel);

        if(gToken == elsesym){
            gGetToken(1);
            ctemp2 = cx; // for jmp
            emit(7, 0, 0, 0, __func__); // JMP
            
            code[ctemp].m = cx;

            gStatement(lexlevel);

            code[ctemp2].m = cx;
        }
        else{
            code[ctemp].m = cx;
        }
    }
    
    if(gToken == whilesym){
        gGetToken(1);
        ctemp = cx; // for condition
        gCondition(lexlevel);
        gGetToken(1);
        
        ctemp2 = cx; // for jump

        emit(8, 0, 0, 0, __func__); // JPC

        gStatement(lexlevel);

        emit(7, 0, 0, ctemp, __func__); // JMP

        code[ctemp2].m = cx; // fixing JPC
    }

    if(gToken == readsym){
        gGetToken(1);

        int tLoc;

        tLoc = gCheckTablev3(gTokenName, lexlevel);

        gGetToken(1);
        
        emit(9, 0, 0, 2, __func__); // READ
        emit(4, 0, lexlevel - gSymbol_table[tLoc].level, 
                gSymbol_table[tLoc].addr, __func__); //STO
    }

    if(gToken == writesym){
        gGetToken(1);
        gExpression(0, lexlevel);
        emit(9, 0, 0, 1, __func__); // WRITE

    }
}

void gCondition(int lexlevel){
    if(gToken == oddsym){
        gGetToken(1);
        gExpression(0, lexlevel);
        emit(15, 0, 0, 0, __func__); // ODD
    }
    else{
        gExpression(0,lexlevel);
        if(gToken == eqsym){
            gGetToken(1);
            gExpression(1, lexlevel);
            emit(17, 0, 0, 1, __func__);
        }

        if(gToken == neqsym){
            gGetToken(1);
            gExpression(1, lexlevel);
            emit(18, 0, 0, 1, __func__); // NEQ
        }

        if(gToken == lessym){
           gGetToken(1); 
           gExpression(1, lexlevel);
           emit(19, 0 , 0, 1, __func__); // LSS
        }

        if(gToken == leqsym){
            gGetToken(1);
            gExpression(1, lexlevel);
            emit(20, 0, 0, 1, __func__); // LEQ
        }

        if(gToken == gtrsym){
            gGetToken(1);
            gExpression(1, lexlevel);
            emit(21, 0, 0, 1, __func__); //GTR
        }

        if(gToken == geqsym){
            gGetToken(1);
            gExpression(1, lexlevel);
            emit(22, 0, 0, 1, __func__); //GEQ
        }
    }
}

void gExpression(int reg, int lexlevel){
    if(gToken == plussym){
        gGetToken(1);
    }

    if(gToken == minussym){
        gGetToken(1);
        gTerm(reg, lexlevel);
        emit(10, reg, 0, 0, __func__); // NEG

        while(gToken == plussym || gToken == minussym){
            if(gToken == plussym){
                gGetToken(1);
                gTerm(reg+1, lexlevel);
                emit(11, reg, reg, reg+1, __func__); // ADD
            }
            if(gToken == minussym){
                gGetToken(1);
                gTerm(reg+1, lexlevel);
                emit(12, reg, reg, reg+1, __func__); // SUB
            }
        }
    }

    gTerm(reg, lexlevel);    

    while(gToken == plussym || gToken == minussym){
        if(gToken == plussym){
            gGetToken(1);
            gTerm(reg+1, lexlevel);
            emit(11, reg, reg, reg+1, __func__); // ADD
        }
        if(gToken == minussym){
            gGetToken(1);
            gTerm(reg+1, lexlevel);
            emit(12, reg, reg, reg+1, __func__); // SUB
        }
    }
}

void gTerm(int reg, int lexlevel){
    gFactor(reg, lexlevel);

    while(gToken == multsym || gToken == slashsym){
        if(gToken == multsym){
            gGetToken(1);
            gFactor(reg+1, lexlevel);
            emit(13, reg, reg, reg+1, __func__); // MUL
        }
        if(gToken == slashsym){
            gGetToken(1);
            gFactor(reg+1, lexlevel);
            emit(14, reg, reg, reg+1, __func__); // DIV
        }
    }
}

void gFactor(int reg, int lexlevel){
    int loc, symIndex;
    if(gToken == identsym){
        // save the symtable index of
        // the var or const entry unmarked and closest in lexlevel

        symIndex = gCheckTablev3(gTokenName, lexlevel);

        symbol curSym = gSymbol_table[symIndex];
        if(curSym.kind == 1){
            // ^emit LIT (1, regtoendupin, 0, value from the symbol table)
            emit(1, reg, 0, curSym.val, __func__); // LIT
        }

        if(curSym.kind == 2){
            // ^emit LOD (3, regtoendupin, L = lexlevel - L from the symbol table, m from the symbol table)
            emit(3, reg, lexlevel - curSym.level, curSym.addr, __func__); // LOD
        }
        gGetToken(1);
    }
    else if(gToken == numbersym){
        emit(1, reg, 0, atoi(gTokenName), __func__); // LIT
        gGetToken(1);
    }
    else if(gToken == semicolonsym){
        //gGetToken(1);
    }
    else{
        gGetToken(1);
        gExpression(reg, lexlevel);
        gGetToken(1);
    }
}


instruction *generate_code(symbol *table,struct entry *list,
        int *tableSize){
    ts = *tableSize;

    gSetHead(list);

    gGenTable(table);

    code = malloc(CODE_SIZE * sizeof(instruction));
    gProgram();

    return code;
}
