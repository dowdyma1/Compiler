#ifndef __CODEGEN_H
#define __CODEGEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parser.h"

    typedef struct instruction{
        int opcode;
        char *op;
        int r;
        int l;
        int m;
    }instruction;

    instruction *generate_code(symbol *table, struct entry *list, int *tableSize);

#ifdef __cplusplus
}
#endif

#endif