#include <stdlib.h>
#include "writeback.h"
#include "memAccess.h"

Writeback *WritebackInit(int **pc, long *reg, MemAccessWriteback *inBucket) {
   Writeback *wb = malloc(sizeof(Writeback));
   return wb;
}
void WritebackPhase(Writeback *writeback) {

}
