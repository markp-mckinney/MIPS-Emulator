#include <stdlib.h>
#include <stdio.h>
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memAccess.h"
#include "writeback.h"
#include "instruction.h"
#define MAX_MEMORY 1000

void printReg(long *reg, Fetch *fPhase, Decode *dPhase, Execute *ePhase, 
      MemAccess *mPhase, Writeback *wPhase){
   int ndx = 0;
   while(ndx < 32) {
      printf("$%d: ", ndx);
      if(ndx < 10)
         printf(" ");
      printf("%08X", (int)reg[ndx]);
      if(ndx++ % 2)
         printf("\n");
      else
         printf("\t");
   }
   printf("\nFetches executed: %d\n", fPhase->count);
   printf("Decodes executed: %d\n", dPhase->count);
   printf("Executes executed: %d\n", ePhase->count);
   printf("Memory Accesses executed: %d\n", mPhase->count);
   printf("Writebacks executed: %d\n", wPhase->count);
}

void run(int *mem, char sr){
   int *pc = mem, instrCount = 0, memRefs = 0, cycles = 0;
   long reg[32] = {0};
   int halt = 0, flush = 0;

   FetchDecode *ifIdBucket = malloc(sizeof(FetchDecode));
   DecodeExecute *idExBucket = malloc(sizeof(DecodeExecute));
   ExecuteMemAccess *exMemBucket = malloc(sizeof(ExecuteMemAccess));
   MemAccessWriteback *memWbBucket = malloc(sizeof(MemAccessWriteback));

   Fetch *fetchPhase = FetchInit(pc, mem, ifIdBucket);
   Decode *decodePhase = DecodeInit(reg, ifIdBucket, idExBucket);
   Execute *executePhase = ExecuteInit(pc, mem, idExBucket, exMemBucket);
   MemAccess *memAccessPhase = MemAccessInit(mem, exMemBucket, memWbBucket);
   Writeback *writebackPhase = WritebackInit(pc, reg, memWbBucket);

   while (sr == 's' && !halt) {
      WritebackPhase(writebackPhase);
      MemAccessPhase(memAccessPhase);
      ExecutePhase(executePhase, &halt, &flush);
      if (flush) {
         ifIdBucket->ready = 0;
         idExBucket->ready = 0;
      }
      DecodePhase(decodePhase);
      FetchPhase(fetchPhase);
      cycles++;

      printReg(reg, fetchPhase, decodePhase, executePhase, memAccessPhase,
            writebackPhase);

      scanf("%c", &sr);
   }
   while (sr == 'r' && !halt) {
      WritebackPhase(writebackPhase);
      MemAccessPhase(memAccessPhase);
      ExecutePhase(executePhase, &halt, &flush);
      if (flush) {
         ifIdBucket->ready = 0;
         idExBucket->ready = 0;
      }
      DecodePhase(decodePhase);
      FetchPhase(fetchPhase);
      cycles++;
   }

   printf("\nInstructions Executed: %d\nMemory References: %d\nClock Cycles: %d\n",
         instrCount, memRefs, cycles);
   printReg(reg, fetchPhase, decodePhase, executePhase, memAccessPhase,
         writebackPhase);
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
