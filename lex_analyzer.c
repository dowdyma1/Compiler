/*
 * Matthew Dowdy
 * 10/22/20
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ctype.h>
#include "lex_analyzer.h"


int toStop = 0;

struct entry* toTable(char cache[10], int tokenType, struct entry* curEntry);

void printTable(struct entry* tableHead);
int checkReserved(char cache[10]);
void printLexemeList(struct entry* cur);
struct entry* doLex(char *filename, bool lflag);
int isStop();
void printSymRep(struct entry* cur);

int isStop(){
    return toStop;
}

struct entry* doLex(char *filename, bool lflag){
    //initialize vars
    FILE * ifp;
    int c;
    char s;
    // cache is where you put the characters to figure out token value of string
    char cache[10] = "";

    // open file
    ifp = fopen(filename, "r");

    // print file
    printf("Source Program:\n");
    s = fgetc(ifp);
    while(feof(ifp) != 1){
        printf("%c",s);
        s = fgetc(ifp);
    }
    fclose(ifp);
    printf("\n");

    // reopen file pointer
    ifp = fopen(filename, "r");

    // initialize table
    struct entry* tableHead = NULL;
    tableHead = (struct entry*)malloc(sizeof(struct entry));
    struct entry* curEntry = tableHead;

    // get character from file
    c = fgetc(ifp);
    // start of logic loop
    while(feof(ifp) != 1){
        // if space or cntrl character, skip
        if(isspace(c) || iscntrl(c)){
            c = fgetc(ifp);
        }
        else{
            // turn c into 12 character array
            char ch[12];
            ch[0] = c;
            ch[1] = '\0';

            // chunk of characters has an alpha in front
            if(isalpha(c)){
                // add to cache
                char y = (char) c;
                const char *x = &y;
                strncat(cache, x, 1);
                c = fgetc(ifp); // increment character from file stream
                
                // as long as we have an alpha or digit, keep adding to cache
                while(isalpha(c) || isdigit(c)){
                    char y = (char) c;
                    const char *x = &y;
                    strncat(cache, x, 1);

                    // if we're over the identifier character limit
                    if(strlen(cache) > 10){
                        while(isalpha(c) || isdigit(c)){ // keep adding chars to cache
                            char y = (char) c;
                            const char *x = &y;
                            strncat(cache, x, 1);
                            c = fgetc(ifp);
                        }
                        printf("Error, identifier too long: %s\n", cache);
                        toStop = 1;
                        return(tableHead);
                        strcpy(cache, "");
                        break;
                    }

                    c = fgetc(ifp);

                }
                int reserved = checkReserved(cache); // check if cache is a reserved word
                if(reserved > 0){
                    curEntry = toTable(cache, reserved, curEntry);
                }
                else{
                    curEntry = toTable(cache, identsym, curEntry);
                }

                strcpy(cache, "");
            }

            // if first char is a digit
            else if(isdigit(c)){
                int isError = 0;
                while(isdigit(c)){
                    char y = (char) c;
                    const char *x = &y;
                    strncat(cache, x, 1);

                    // if cache bigger than 5
                    if(strlen(cache) > 5){
                        while(isdigit(c)){
                            char y = (char) c;
                            const char *x = &y;
                            strncat(cache, x, 1);
                            c = fgetc(ifp);
                        }
                        printf("error, number too big: %s\n", cache);
                        toStop = 1;
                        return(tableHead);
                        strcpy(cache, "");
                        isError = 1;
                        break;
                    }

                    c = fgetc(ifp);

                    // if theres an alpha in this number
                    if(isalpha(c)){
                        isError = 1;
                        while(isalpha(c)){
                            char y = (char) c;
                            const char *x = &y;
                            strncat(cache, x, 1);
                            c = fgetc(ifp);
                        }
                        printf("error, invalid identifier: %s\n", cache);
                        toStop = 1;
                        return(tableHead);
                        strcpy(cache, "");
                    }
                }
                // if we have an error, don't add it to table
                if(!isError){
                    curEntry = toTable(cache, numbersym, curEntry);
                    strcpy(cache, "");
                }
                isError = 0;
            }

            // check if char is one character token & add to table
            else if(c == '+'){
                curEntry = toTable(ch, plussym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == '-'){
                curEntry = toTable(ch, minussym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == '*'){
                curEntry = toTable(ch, multsym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == '='){
                curEntry = toTable(ch, eqsym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == '('){
                curEntry = toTable(ch, lparentsym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == ')'){
                curEntry = toTable(ch, rparentsym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == ','){
                curEntry = toTable(ch, commasym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == ';'){
                curEntry = toTable(ch, semicolonsym, curEntry);
                c = fgetc(ifp);
            }
            else if(c == '.'){
                curEntry = toTable(ch, periodsym, curEntry);
                c = fgetc(ifp);
            }

            // two character tokens
            else if(c == '<'){
                char y = (char) c;
                const char *x = &y;
                strncat(cache, x, 1);
                ch[0] = c;
                c = fgetc(ifp);
                if(c == '>'){
                    char y = (char) c;
                    const char *x = &y;
                    strncat(cache, x, 1);
                    curEntry = toTable(cache, neqsym, curEntry);
                    c = fgetc(ifp);
                }
                else if(c == '='){
                    char y = (char) c;
                    const char *x = &y;
                    strncat(cache, x, 1);
                    curEntry = toTable(cache, leqsym, curEntry);
                    c = fgetc(ifp);
                }
                else{
                    curEntry = toTable(ch, lessym, curEntry);
                }
                strcpy(cache, "");
            }

            // more two character tokens
            else if(c == '>'){
                char y = (char) c;
                const char *x = &y;
                strncat(cache, x, 1);
                ch[0] = c;
                c = fgetc(ifp);
                if(c == '='){
                    char y = (char) c;
                    const char *x = &y;
                    strncat(cache, x, 1);
                    curEntry = toTable(cache, geqsym, curEntry);
                    c = fgetc(ifp);
                }
                else{
                    curEntry = toTable(ch, gtrsym, curEntry);
                }
                strcpy(cache, "");
            }

            // :=
            else if(c == ':'){
                char y = (char) c;
                const char *x = &y;
                strncat(cache, x, 1);
                c = fgetc(ifp);
                if(c == '='){
                    char y = (char) c;
                    const char *x = &y;
                    strncat(cache, x, 1);
                    curEntry = toTable(cache, becomessym, curEntry);
                    c = fgetc(ifp);
                }
                else{
                    printf("invalid symbol error, :\n");
                    toStop = 1;
                    return(tableHead);
                }
                strcpy(cache, "");
            }

            // check if we have comment or slashsym
            else if(c == '/'){
                int next = fgetc(ifp);
                if(next == '*'){
                    int first, second;
                    first = fgetc(ifp);
                    second = fgetc(ifp);
                    while(feof(ifp) != 1){
                        if(first == '*' && second == '/'){break;}
                        else{
                            first = second;
                            second = fgetc(ifp);
                        }
                    }
                    if(feof(ifp)!=1){c = fgetc(ifp);}
                    else{break;}
                }
                else{
                    ch[0] = c;
                    curEntry = toTable(ch, slashsym, curEntry);
                    c = next;
                }
                strcpy(cache, "");
            }
            else{
                printf("invalid character error, %c.\n", c);
                toStop = 1;
                return(tableHead);
                c = fgetc(ifp);
            }
        }
    }


    if(lflag){
        printf("\n");
        printTable(tableHead);
        printf("\nLexeme List:\n");
        printLexemeList(tableHead);
        printf("\nSymbolic representation:\n");
        printSymRep(tableHead);
        printf("\n");
    }

    fclose(ifp);
    //free(tableHead);

    return tableHead;
}

// table is a linked list, returns the next entry
struct entry* toTable(char cache[10], int tokenType, struct entry* curEntry){
    strcpy(curEntry->lexeme, cache);
    curEntry->tokenType = tokenType;
    struct entry* nextEntry = NULL;
    nextEntry = (struct entry*)malloc(sizeof(struct entry));
    curEntry->next = nextEntry;

    return nextEntry;
}

// cycles through linked list and prints contents
void printTable(struct entry* tableHead){
    struct entry* cur = tableHead;

    printf("lexeme\t\t\ttoken type\n");

    while(cur->next != NULL){
        printf("%s\t\t\t%d\n", cur->lexeme, cur->tokenType);
        cur = cur->next;
    }

}

// maps token string to token number for reserved words
int checkReserved(char cache[10]){
    if(strcmp(cache, "const") == 0){return constsym;}
    if(strcmp(cache, "var") == 0){return varsym;}
    if(strcmp(cache, "procedure") == 0){return procsym;}
    if(strcmp(cache, "call") == 0){return callsym;}
    if(strcmp(cache, "begin") == 0){return beginsym;}
    if(strcmp(cache, "end") == 0){return endsym;}
    if(strcmp(cache, "if") == 0){return ifsym;}
    if(strcmp(cache, "then") == 0){return thensym;}
    if(strcmp(cache, "else") == 0){return elsesym;}
    if(strcmp(cache, "while") == 0){return whilesym;}
    if(strcmp(cache, "do") == 0){return dosym;}
    if(strcmp(cache, "read") == 0){return readsym;}
    if(strcmp(cache, "write") == 0){return writesym;}
    if(strcmp(cache, "odd") == 0){return oddsym;}

    return 0;
}

// prints lexeme list
void printLexemeList(struct entry* cur){
    while(cur->next != NULL){
        if(cur->tokenType == 2 || cur->tokenType == 3){
            printf("%d %s ", cur->tokenType, cur->lexeme);
        }
        else{
            printf("%d ", cur->tokenType);
        }
        cur = cur->next;
    }
    printf("\n");
}

char * intToCharSym(int iToken){
    switch(iToken){
        case 1:
            return("nulsym");
        case 2:
            return("identsym");
        case 3:
            return("numbersym");
        case 4:
            return("plussym");
        case 5:
            return("minussym");
        case 6:
            return("multsym");
        case 7:
            return("slashsym");
        case 8:
            return("oddsym");
        case 9:
            return("eqlsym");
        case 10:
            return("neqsym");
        case 11:
            return("lessym");
        case 12:
            return("leqsym");
        case 13:
            return("gtrsym");
        case 14:
            return("geqsym");
        case 15:
            return("lparentsym");
        case 16:
            return("rparentsym");
        case 17:
            return("commasym");
        case 18:
            return("semicolonsym");
        case 19:
            return("periodsym");
        case 20:
            return("becomessym");
        case 21:
            return("beginsym");
        case 22:
            return("endsym");
        case 23:
            return("ifsym");
        case 24:
            return("thensym");
        case 25:
            return("whilesym");
        case 26:
            return("dosym");
        case 27:
            return("callsym");
        case 28:
            return("constsym");
        case 29:
            return("varsym");
        case 30:
            return("procsym");
        case 31:
            return("writesym");
        case 32:
            return("readsym");
        case 33:
            return("elsesym");
        
    }
    return "";
}

void printSymRep(struct entry* cur){
    while(cur->next != NULL){
        if(cur->tokenType == 2 || cur->tokenType == 3){
            printf("%s %s ", intToCharSym(cur->tokenType), cur->lexeme);
        }
        else{
            printf("%s ", intToCharSym(cur->tokenType));
        }
        cur = cur->next;
    }
    printf("\n");
}