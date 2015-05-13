#ifndef EXECUTE_H
#define EXECUTE_H

#include "decode.h"
#include "instruction.h"

#define OPERATION_READ  1
#define OPERATION_WRITE 2
#define OPERATION_NONE  0

/**
 * Out-bucket for the execute phase, in-bucket for the memory access phase.
 * Given the inputs, figures out whatever the result should be.  If some value
 * needs to be written or read to memory, this struct should contain info
 * describing the operation that needs to be done.
 */
typedef struct {
   int ready;
   int address;
   int reg;
   int operation;
   int value;
} ExecuteMemAccess;


typedef struct {
   int count;
   DecodeExecute *inBucket;
   ExecuteMemAccess *outBucket;
   int **pc;
   int *mem;
} Execute;


Execute *ExecuteInit(int **pc, int *mem, DecodeExecute *inBucket,
      ExecuteMemAccess *outBucket);

/**
 * Now that all values have been resolved, do the operation.
 * If it's a memory read or write, you can't do that here.  You need to
 * notify the memory access phase that something needs to be done by 
 * writing to the out bucket.
 */
void ExecutePhase(Execute *execute, int *halt, int *flush);

#endif

