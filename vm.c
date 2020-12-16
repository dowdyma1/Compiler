/*
 * Matthew Dowdy
 * 09/24/20
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vm.h"
#include "codegen.h"

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

int arLens[MAX_CODE_LENGTH];
int arCount = 0;

void virtual_machine(instruction *code, int size, bool aflag, bool vflag);
char *opcode_to_str(int opcode);
void print_arr(int *arr, int size);
void print_stack(int *stack);
int base(int l, int base, int *stack);

typedef struct {
    int op; //opcode
    int r; // R
    int l; // L
    int m; // M
}instruct;

// helper fn to get string representation of opcodes
char* opcode_to_str(int opcode) {
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


// helper fn to print array given the array and size
void print_arr(int *arr, int size){
    for(int i = 0; i < size; i++){
        printf("%d", arr[i]);
        if(i != size-1){printf(" ");}
    }

    return;
}

// helper fn to print stack
void print_stack(int *stack){
    int curAr = 0;
    int curBp = MAX_STACK_HEIGHT-1;
    
    while(curAr < arCount){
        if(curAr != 0){printf("| ");}

        for(int i = curBp; i > (curBp-arLens[curAr]); i--){
            printf("%d ", stack[i]);
        }
        curBp = curBp - arLens[curAr];
        curAr++;
    }

    return;
}

// provided code
int base(int l, int base, int *stack){
    int b1; // find base L levels up
    b1 = base;

    while(l > 0){
        b1 = stack[b1];
        l--;
    }
    return b1;
}


void virtual_machine(instruction *code, int size, bool aflag, bool vflag){
    
    // initial values
    int i = 0;
    int sp = MAX_STACK_HEIGHT;
    int bp = sp - 1;
    int pc = 0;
    instruct ir;
    instruct text[MAX_CODE_LENGTH];
    int *stack = (int*)malloc(MAX_STACK_HEIGHT * sizeof(int));
    int temp_rf[] = {0,0,0,0,0,0,0,0};
    int *rf = temp_rf;
    int line_num = 0;

    // set initial values for stack to zero
    for(i = 0; i < MAX_STACK_HEIGHT; i++){
        stack[i] = 0;
    }

    for(i = 0; i < size; i++){
        text[i].op = code[i].opcode;
        text[i].r = code[i].r;
        text[i].l = code[i].l;
        text[i].m = code[i].m;
    }

    if(aflag){
        // printing input
        printf("\nLine OP  R L M\n");
        for(int j = 0; j < i; j++){
            if(j<10){printf("%d    ", j);}
            if(j>=10 && i<100){printf("%d   ", j);}
            if(j>100){printf("%d  ", j);}
            printf("%s %d %d %d\n", 
                    opcode_to_str(text[j].op), text[j].r, 
                    text[j].l, text[j].m);
        }
    }

    //initialize IR
    ir.op = 0;
    ir.r = 0;
    ir.l = 0;
    ir.m = 0;

    
    if(vflag){
        // PRINTING INITIAL VALUES
        printf("\t\t\t\t\tpc\tbp\tsp\n");
        printf("\t\t\t\t\t%d\t%d\t%d\n",pc,bp,sp);
        printf("Initial values\nRegisters: ");
        print_arr(rf,8);
        printf("\nStack: ");
        printf("\n");
    }

    // main code loop
    while(!(ir.op == 9 && ir.m == 3)){ // while not eop
        if((sp < 0) || bp < 0){
            printf("uh oh... neg sp or bp.\n");
            break;
        }
        if(arCount == 6){break;}

        // FETCH
        ir.op = text[pc].op;
        ir.r = text[pc].r;
        ir.l = text[pc].l;
        ir.m = text[pc].m;
        line_num = pc;
        pc++;

        // EXECUTE
        switch(ir.op){
            case 1:
                rf[ir.r] = ir.m;
                break;
            case 2:
                sp = bp + 1;
                bp = stack[sp - 2];
                pc = stack[sp - 3];
                arCount--;
                break;
            case 3:
                rf[ir.r] = stack[base(ir.l, bp, stack) - ir.m];
                break;
            case 4:
                stack[base(ir.l, bp, stack) - ir.m] = rf[ir.r];
                break;
            case 5:
                stack[sp - 1] = base(ir.l, bp, stack);
                stack[sp - 2] = bp;
                stack[sp - 3] = pc;
                bp = sp -1;
                pc = ir.m;
                break;
            case 6:
                sp = sp - ir.m;
                arLens[arCount] = bp - sp +1;
                arCount++;
                break;
            case 7:
                pc = ir.m;
                break;
            case 8:
                if(rf[ir.r] == 0){pc = ir.m;}
                break;
            case 9:
                if(ir.m == 1){
                    printf("\nRegister %d: %d\n",ir.r, rf[ir.r]);
                }
                if(ir.m == 2){
                    printf("\nenter integer: ");
                    scanf("%d", &rf[ir.r]);
                }
                if(ir.m == 3){break;}
                break;
            case 10:
                rf[ir.r] = -1*rf[ir.r];
                break;
            case 11:
                rf[ir.r] = rf[ir.l] + rf[ir.m];
                break;
            case 12:
                rf[ir.r] = rf[ir.l] - rf[ir.m];
                break;
            case 13:
                rf[ir.r] = rf[ir.l] * rf[ir.m];
                break;
            case 14:
                rf[ir.r] = rf[ir.l] / rf[ir.m];
                break;
            case 15:
                rf[ir.r] = rf[ir.r] % 2;
                break;
            case 16:
                rf[ir.r] = rf[ir.l] % rf[ir.m];
                break;
            case 17:
                if(rf[ir.l] == rf[ir.m]){
                    rf[ir.r] = 1;
                }
                else{
                    rf[ir.r] = 0;
                }
                break;
            case 18:
                if(rf[ir.l] != rf[ir.m]){
                    rf[ir.r] = 1;
                }
                else{
                    rf[ir.r] = 0;
                }
                break;
            case 19:
                if(rf[ir.l] < rf[ir.m]){
                    rf[ir.r] = 1;
                }
                else{
                    rf[ir.r] = 0;
                }
                break;
            case 20:
                if(rf[ir.l] <= rf[ir.m]){
                    rf[ir.r] = 1;
                }
                else{
                    rf[ir.r] = 0;
                }
                break;
            case 21:
                if(rf[ir.l] > rf[ir.m]){
                    rf[ir.r] = 1;
                }
                else{
                    rf[ir.r] = 0;
                }
                break;
            case 22:
                if(rf[ir.l] >= rf[ir.m]){
                    rf[ir.r] = 1;
                }
                else{
                    rf[ir.r] = 0;
                }
                break;

        }
        
        if(vflag){
            // PRINTING
            printf("\n");
            printf("\t\t\t\t\tpc\tbp\tsp\n");
            printf("%d %s %d %d %d\t\t\t\t%d\t%d\t%d\n",
                    line_num, opcode_to_str(ir.op),ir.r, ir.l,ir.m,
                    pc,bp,sp);
            printf("Registers: ");
            print_arr(rf,8);
            printf("\nStack: ");
            print_stack(stack);
            printf("\n");
        }

    }

    free(stack);

}