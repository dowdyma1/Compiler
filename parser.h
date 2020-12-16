#ifndef __PARSER_H
#define __PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SYMBOL_TABLE_SIZE 500

    typedef struct symbol{
        int kind;       // const = 1; var = 2; proc = 3
        char name[10];  // name
        int val;        // ASCII val
        int level;      // L level
        int addr;       // M address
        bool isMarked;
    }symbol;

    symbol* parse(struct entry *list);

#ifdef __cplusplus
}
#endif

#endif