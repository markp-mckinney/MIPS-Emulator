#ifndef DECODE_H
#define DECODE_H

#include "fetch.h"

/**
 * Out-bucket for the decode phase, in-bucket for the execute phase.
 * Contains the values stored in the relevant registers, as well as enough
 * information to tell the later phases what to do (opcode, funct code, 
 * destination, I think that's it..._
 */
typedef struct {
   int ready;
   int target;
   int source;
   int destination;
   int opcode;
} DecodeExecute;


typedef struct {
   int count;
   FetchDecode *inBucket;
   DecodeExecute *outBucket;
   long *reg;
} Decode;


Decode *DecodeInit(long *reg, FetchDecode *inBucket, DecodeExecute *outBucket);

/**
 * Decode phase splits instruction into it's corresponding sections and looks
 * up register values in the register file.
 */
void DecodePhase(Decode *decode);

#endif

