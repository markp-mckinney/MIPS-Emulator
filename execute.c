#include <stdlib.h>

#include "execute.h"

#define R 0
#define J 2
#define JAL 3
#define MASK 31
                     //{"and","or","add","addu","sll","srl","sra","sub","sltu","jr","syscall"};
const int R_CODES[] = {0x24,0x25,0x20,0x21,0x00,0x02,0x03,0x22,0x2B,0x08,0xC};
                     //{"addi", "addiu", "sltiu", "beq", "bne", "lw", "sw", "ori", "lui"};
const int I_CODES[] = {0x08,0x09,0x0B,4,5,0x23,0x2B,0x0D,0x0F};

Execute *ExecuteInit(int **pc, int *mem, DecodeExecute *inBucket,
      ExecuteMemAccess *outBucket) {
   Execute *res = malloc(sizeof(Decode));

   res->count = 0;
   res->inBucket = inBucket;
   res->outBucket = outBucket;
   res->pc = pc;
   res->mem = mem;

   return res;
}

/**
 * Now that all values have been resolved, do the operation.
 * If it's a memory read or write, you can't do that here.  You need to
 * notify the memory access phase that something needs to be done by 
 * writing to the out bucket.
 */
void ExecutePhase(Execute *execute, int *halt, int *flush) {
   DecodeExecute *instr = execute->inBucket;
   ExecuteMemAccess *out = execute->outBucket;

   if (instr->ready == 0)
      return;
   instr->ready = 0;

   int func, br;

   execute->count++;
   switch (instr->opcode){
      case R:
         for(func = 0, br = 1; br;)
            if(R_CODES[func] == instr->funct)
               br = 0;
            else func++;
         switch (func){
            case 0: //and
               out->value = instr->rs & instr->rt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 1: //or
               out->value = instr->rs | instr->rt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 2: //add
               out->value = instr->rs + instr->rt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 3: //addu
               out->value = instr->rs + instr->rt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 4: //sll
               out->value = instr->rt << instr->shamt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 5: //srl
               out->value = (unsigned int)instr->rt >> instr->shamt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 6: //sra
               out->value = instr->rt >> instr->shamt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 7: //sub
               out->value = instr->rs - instr->rt;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 8: //sltu
               out->value = ((unsigned int)instr->rs < (unsigned int)instr->rt) ? 1 : 0;
               out->reg = instr->rd;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 9: //jr
               out->value = **(execute->pc);
               out->reg = 31;
               *flush = 1;
               *execute->pc = execute->mem + instr->rs;
               out->operation = OPERATION_NONE;
               break;
            case 10: //syscall (assume halt)
               *halt = 1;
               break;
         }
         break;
      case J:
         *execute->pc = execute->mem + instr->imm;
         *flush = 1;
         break;
      case JAL:
         out->value = **(execute->pc);
         out->reg = 31;
         *flush = 1;
         *execute->pc = execute->mem + instr->imm;
         out->operation = OPERATION_NONE;
         out->ready = 1;
         break;
      default:
         for(func = 0, br = 1; br;)
            if(I_CODES[func] == instr->opcode)
               br = 0;
            else func++;
         switch (func){
            case 0: //addi
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               out->value = instr->rs + instr->imm;
               out->reg = instr->rt;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 1: //addiu
               out->value = (unsigned int)instr->rs + (unsigned int)instr->imm;
               out->reg = instr->rt;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 2: //sltiu
               out->value = instr->rs < instr->imm ? 1 : 0;
               out->reg = instr->rt;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 3: //beq
               if(instr->imm >> 15) instr->imm |= 0xFFFFFFFFFFFF0000;
               if(instr->rs == instr->rt) *(execute->pc) += instr->imm - 1;
               out->reg = instr->rt;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 4: //bne
               if(instr->imm >> 15) instr->imm |= 0xFFFFFFFFFFFF0000;
               if(instr->rs != instr->rt) *(execute->pc) += instr->imm - 1;
               out->operation = OPERATION_NONE;
               out->ready = 1;
               break;
            case 5: //lw
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               out->address = instr->rs + instr->imm;
               out->reg = instr->rt;
               out->operation = OPERATION_READ;
               out->ready = 1;
               break;
            case 6: //sw
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               out->address = instr->rs + instr->imm;
               out->value = instr->rt;
               out->operation = OPERATION_WRITE;
               out->ready = 1;
               break;
            case 7: //ori
               out->value = instr->rs | instr->imm;
               out->reg = instr->rt;
               out->ready = 1;
               break;
            case 8: //lui
               out->value = instr->imm << 16;
               out->reg = instr->rt;
               out->ready = 1;
               break;
         }
         break;
   }
}

