#ifndef FETCH_H
#define FETCH_H

/**
 * Out-bucket for the fetch phase, in-bucket for the decode phase.
 * Just stores the machine code representation of the instruction
 * as a 32-bit integer... the decode phase will split it up.
 */
typedef struct {
   int ready;
   int instruction;
} FetchDecode;


typedef struct {
   int count;
   FetchDecode *outBucket;
   int **pc;
} Fetch;


Fetch *FetchInit(int **pc, FetchDecode *outBucket);

/**
 * All the fetch phase does is read an instruction out of memory, put it in the
 * out bucket, and set the ready flag to 1.
 */
void FetchPhase(Fetch *fetch);

#endif
