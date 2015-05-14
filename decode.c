#include <stdio.h>
#include <stdlib.h>

#include "decode.h"
#include "fetch.h"

#define R 0
#define J 2
#define JAL 3
#define MASK 31

Decode *DecodeInit(long *reg, FetchDecode *inBucket,
      DecodeExecute *outBucket) {
   Decode *res = malloc(sizeof(Decode));

   res->count = 0;
   res->inBucket = inBucket;
   res->outBucket = outBucket;
   res->reg = reg;

   return res;
}

/**
 * Decode phase splits instruction into it's corresponding sections and looks
 * up register values in the register file.
 */
void DecodePhase(Decode *decode) {
   FetchDecode *in = decode->inBucket;
   DecodeExecute *out = decode->outBucket;
   int instruction = decode->inBucket->instruction;

   if (!in->ready)
      return;
   in->ready = 0;

   if (instruction == 0) {
	   return;
   }

   out->opcode = instruction >> 26 & 0x3F;
   switch (out->opcode){
      case R:
         out->rs = decode->reg[instruction >> 21 & MASK];
         out->rt = decode->reg[instruction >> 16 & MASK];
         out->rd = instruction >> 11 & MASK;
         out->shamt = instruction >> 6 & MASK;
         out->funct = instruction & 63;
         break;
      case J: case JAL:
         out->imm = instruction & 0x03FFFFFF;
         break;
      default:
         out->rs = decode->reg[instruction >> 21 & MASK];
         out->rt = instruction >> 16 & MASK;
		 out->rtval = decode->reg[out->rt];
         out->imm = instruction & 0x0000FFFF;
         break;
   }

   decode->count++;
   out->ready = 1;
}

