#include <stdlib.h>
#include "memAccess.h"
#include "execute.h"

MemAccess *MemAccessInit(int *pc, ExecuteMemAccess *inBucket,
   MemAccessWriteback *outBucket) {
   
   MemAccess *memacc = malloc(sizeof(MemAccess));
   return memacc;
}

void MemAccessPhase(MemAccess *memAccess) {

}
