#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct {
   short opcode;
   short rs;
   short rt;
   short rd;
   short shamt;
   short funct;
   int imm;
} Instruction;

void decode(Instruction *instr, int *pc);

int execute(Instruction *instr, int *pc, int *reg, int *mem, int *cycles, int *memRefs);

#endif