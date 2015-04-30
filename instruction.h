#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct {
   short opcode;
   short rs;
   short rt;
   short rd;
   short shamt;
   short funct;
   long imm;
} Instruction;

void decode(Instruction *instr, int *pc);

int execute(Instruction *instr, int **pc, long *reg, int *mem, int *cycles, int *memRefs);

#endif