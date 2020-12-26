#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lex_analyzer.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

struct entry* doLex(char *filename, bool lflag);
int isStop();
symbol * parse(struct entry *list);
int tableSize();
int isHalt();
instruction *generate_code(symbol *table, struct entry *list, int *tableSize);
int getCodeSize();
void virtual_machine(instruction *code, int size, bool aflag, bool vflag);



int main(int argc, char *argv[]){
    
    /*
    if(argv[2][0] == 'v' || argv[2][0] == 'a' || argv[2][0] == 'l' ||
            argv[2][0] == '\0'){
        printf("Please include the file name.\n");
        return 1;
    }
    */

    bool aFlag = false, vFlag = false, lFlag = false;

    if(argc == 5){
        aFlag = true;
        vFlag = true;
        lFlag = true;
    }
    else if(argc != 2){
        if(argv[2][1] == 'v'){
            vFlag = true;
        }
        else if(argv[2][1] == 'a'){
            aFlag = true;
        }
        else{
            lFlag = true;
        }

        if(argc == 4){
            if(argv[3][1] == 'v'){
                vFlag = true;
            }
            else if(argv[3][1] == 'a'){
                aFlag = true;
            }
            else{
                lFlag = true;
            }
        }
    }


    struct entry *list = doLex(argv[1], lFlag); // from lex_analyzer.c

    int toQuit = isStop();

    if(toQuit){return 0;}

    symbol * table = parse(list);
    int size = tableSize();
    int toHalt = isHalt();

    if(toHalt){return 0;}

    printf("No errors, program is syntactically correct.\n");

    instruction *code = generate_code(table, list, &size);

    int codeSize = getCodeSize();

    virtual_machine(code, codeSize, aFlag, vFlag);

    free(list);
    free(table);
    free(code);


    return 0;
}
