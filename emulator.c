#include <stdio.h>
#include "instruction.h"
#define MAX_MEMORY 1000

void printReg(long *reg){
   int ndx = 0;
   while(ndx < 32){
      printf("$%d: %X", ndx, (int)reg[ndx]);
      if(ndx++ % 2)
         printf("\n");
      else
         printf("\t");
   }
}

void run(int *mem, char sr){
   int *pc = mem, instrCount = 0, memRefs = 0, cycles = 0;
   long reg[32] = {0};
   Instruction instr;
   if(sr == 'r'){
      for(decode(&instr, pc); execute(&instr, &pc, reg, mem, &cycles, &memRefs); decode(&instr, pc))
         instrCount++;
      instrCount++;
      printReg(reg);
   }
   else
      for(scanf("%c", &sr), decode(&instr, pc); sr != 'q' && execute(&instr, &pc, reg, mem, &cycles, &memRefs); scanf("%c", &sr), decode(&instr, pc)){
         if(sr == 'r'){
            for(decode(&instr, pc); execute(&instr, &pc, reg, mem, &cycles, &memRefs); decode(&instr, pc))
               instrCount++;
            sr = 'q';
         }
         instrCount++;
         printReg(reg);
      }

   printf("\nInstructions Executed: %d\nMemory References: %d\nClock Cycles: %d\n", instrCount, memRefs, cycles);
}

int main(int argc, char **argv){
    if(argc == 3 && *argv[1] == '-' && (*(argv[1] + 1) == 's' || *(argv[1] + 1) == 'r')){
        FILE *file;
        int mem[MAX_MEMORY];
        int *tmp = mem;
        file = fopen(argv[2], "r");
        if(file){
            while(fscanf(file, "%X", tmp++) != EOF)
                ;
            run(mem, *(argv[1] + 1));
        }
    }
    else
        printf("Usage: ./spim <-step or -run> <file>\n");
    return 0;
}
