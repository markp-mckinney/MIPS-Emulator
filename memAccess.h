#ifndef MEM_ACCESS_H
#define MEM_ACCESS_H

#include "execute.h"

/**
 * Memory Access phase is the phase that interracts with memory.  
 * This phase needs to know whether a read or a write is happening, the 
 * address in memory to read or write to/from, and the register to get/put
 * the result into/from.
 *
 * In the event of a write to memory, no registers need to be updated.  Don't
 * set the ready bit in the out bucket.
 */

/**
 * Out-bucket for the execute phase, in-bucket for the memory access phase.
 * Given the inputs, figures out whatever the result should be.  If some value
 * needs to be written or read to memory, this struct should contain info
 * describing the operation that needs to be done.
 */
typedef struct {
   int ready;
   int reg;
   int val;
} MemAccessWriteback;


typedef struct {
   int count;
   DecodeExecute *inBucket;
   ExecuteMemAccess *outBucket;
} MemAccess;


MemAccess *MemAccessInit(int *pc, ExecuteMemAccess *inBucket,
      MemAccessWriteback *outBucket);

void MemAccessPhase(MemAccess *memAccess);

#endif

