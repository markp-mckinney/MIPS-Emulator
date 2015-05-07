#ifndef WRITEBACK_H
#define WRITEBACK_H

#include "memAccess.h"

/**
 * Writeback phase has no out bucket.
 * Writeback phase is responsible for taking the result of the operation and
 * writing it to a register.
 */

typedef struct {
   int count;
   MemAccessWriteback *inBucket;
   long *reg;
   int *pc;
} Writeback;


Writeback *WritebackInit(int *pc, long *reg, MemAccessWriteback *inBucket);

void WritebackPhase(Writeback *writeback);

#endif

