#include <stdlib.h>
#include "memAccess.h"
#include "execute.h"

MemAccess *MemAccessInit(int *mem, ExecuteMemAccess *inBucket,
   MemAccessWriteback *outBucket) {
   
   MemAccess *memacc = malloc(sizeof(MemAccess));
   memacc->mem = mem;
   return memacc;
}

void MemAccessPhase(MemAccess *memAccess) {
   ExecuteMemAccess *in = memAccess->inBucket;
   MemAccessWriteback *out = memAccess->outBucket;

   if (!in->ready)
      return;
   in->ready = 0;

   memAccess->count++;

   switch (in->operation) {
      case OPERATION_READ:
         out->ready = 1;
         out->reg = in->reg;
         out->val = memAccess->mem[in->address];
         break;
      case OPERATION_WRITE:
         memAccess->mem[in->address] = in->value;
         break;
      case OPERATION_NONE:
         out->ready = 1;
         out->reg = in->reg;
         out->val = in->value;
         break;
   }
}
