#include <stdlib.h>
#include "memAccess.h"
#include "execute.h"

MemAccess *MemAccessInit(int *mem, ExecuteMemAccess *inBucket,
   MemAccessWriteback *outBucket) {
   
   MemAccess *memacc = malloc(sizeof(MemAccess));
   return memacc;
}

void MemAccessPhase(MemAccess *memAccess) {

}
