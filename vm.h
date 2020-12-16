#ifndef __VM_H
#define __VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "codegen.h"


void virtual_machine(instruction *code, int size, bool aflag, bool vflag);



#ifdef __cplusplus
}
#endif

#endif