#include "instruction.h"

#define R 0
#define J 2
#define JAL 3
#define MASK 31
                     //{"and","or","add","addu","sll","srl","sra","sub","sltu","jr","syscall"};
const int R_FUNC_CODES[] = {0x24,0x25,0x20,0x21,0x00,0x02,0x03,0x22,0x2B,0x08,0xC};
                     //{"addi", "addiu", "sltiu", "beq", "bne", "lw", "sw", "ori", "lui"};
const int I_OPCODES[] = {0x08,0x09,0x0B,4,5,0x23,0x2B,0x0D,0x0F};

void decode(Instruction *instr, int *pc){
   instr->opcode = *pc >> 26 & 0x3F;
   switch (instr->opcode){
      case R:
         instr->rs = *pc >> 21 & MASK;
         instr->rt = *pc >> 16 & MASK;
         instr->rd = *pc >> 11 & MASK;
         instr->shamt = *pc >> 6 & MASK;
         instr->funct = *pc & 63;
         break;
      case J: case JAL:
         instr->imm = *pc & 0x03FFFFFF;
         break;
      default:
         instr->rs = *pc >> 21 & MASK;
         instr->rt = *pc >> 16 & MASK;
         instr->imm = *pc & 0x0000FFFF;
         break;
   }
}

// returns 1 if executed, 0 if there is a halt instruction
int execute(Instruction *instr, int **pc, long *reg, int *mem, int *cycles, int *memRefs){
   int func, br, ret = 1;
   (*pc)++;
   switch (instr->opcode){
      case R:
         for(func = 0, br = 1; br;)
            if(R_FUNC_CODES[func] == instr->funct)
               br = 0;
            else func++;
         switch (func){
            case 0: //and
               reg[instr->rd] = reg[instr->rs] & reg[instr->rt];
               break;
            case 1: //or
               reg[instr->rd] = reg[instr->rs] | reg[instr->rt];
               break;
            case 2: //add
               reg[instr->rd] = reg[instr->rs] + reg[instr->rt];
               break;
            case 3: //addu
               reg[instr->rd] = reg[instr->rs] + reg[instr->rt];
               break;
            case 4: //sll
               reg[instr->rd] = reg[instr->rt] << instr->shamt;
               break;
            case 5: //srl
               reg[instr->rd] = (unsigned int)reg[instr->rt] >> instr->shamt;
               break;
            case 6: //sra
               reg[instr->rd] = reg[instr->rt] >> instr->shamt;
               break;
            case 7: //sub
               reg[instr->rd] = reg[instr->rs] - reg[instr->rt];
               break;
            case 8: //sltu
               reg[instr->rd] = ((unsigned int)reg[instr->rs] < (unsigned int)reg[instr->rt]) ? 1 : 0;
               break;
            case 9: //jr
               reg[31] = (long)*pc;
               *pc = (int *)reg[instr->rs];
               break;
            case 10: //syscall (assume halt)
               ret = 0;
               break;
         }
         break;
      case J:
         *pc += instr->imm / 4;
         break;
      case JAL:
         reg[31] = (long)*pc;
         *pc += instr->imm / 4;
         break;
      default:
         for(func = 0, br = 1; br;)
            if(I_OPCODES[func] == instr->opcode)
               br = 0;
            else func++;
         switch (func){
            case 0: //addi
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               reg[instr->rt] = reg[instr->rs] + instr->imm;
               break;
            case 1: //addiu
               reg[instr->rt] = (unsigned int)reg[instr->rs] + (unsigned int)instr->imm;
               break;
            case 2: //sltiu
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               reg[instr->rt] = reg[instr->rs] < instr->imm ? 1 : 0;
               break;
            case 3: //beq
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               if(reg[instr->rs] == reg[instr->rt]) *pc += instr->imm / 4;
               break;
            case 4: //bne
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               if(reg[instr->rs] != reg[instr->rt]) *pc += instr->imm / 4;
               break;
            case 5: //lw
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               reg[instr->rt] = mem[reg[instr->rs] + instr->imm];
               (*memRefs)++;
               break;
            case 6: //sw
               if(instr->imm >> 15) instr->imm |= 0xFFFF0000;
               mem[reg[instr->rs] + instr->imm] = reg[instr->rt];
               (*memRefs)++;
               break;
            case 7: //ori
               reg[instr->rt] = reg[instr->rs] | instr->imm;
               break;
            case 8: //lui
               reg[instr->rt] = instr->imm << 16;
               break;
         }
         break;
   }
   return ret;
}