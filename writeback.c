#include <stdio.h>
#include <stdlib.h>
#include "writeback.h"
#include "memAccess.h"

Writeback *WritebackInit(int **pc, long *reg, MemAccessWriteback *inBucket) {
   Writeback *wb = malloc(sizeof(Writeback));

   wb->count = 0;
   wb->inBucket = inBucket;
   wb->reg = reg;
   wb->pc = pc;

   return wb;
}
void WritebackPhase(Writeback *writeback) {
   MemAccessWriteback *in = writeback->inBucket;

   if (!in->ready) {
      return;
   }
   in->ready = 0;

   writeback->reg[in->reg] = in->val;
   writeback->count++;
}
