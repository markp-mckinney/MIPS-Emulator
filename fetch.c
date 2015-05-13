#include <stdlib.h>
#include "fetch.h"

Fetch *FetchInit(int **pc, FetchDecode *outBucket) {
   Fetch *fetch = malloc(sizeof(Fetch));
   fetch->count = 0;
   fetch->outBucket = outBucket;
   fetch->pc = pc;
   return fetch;
}

/**
 * All the fetch phase does is read an instruction out of memory, put it in the
 * out bucket, and set the ready flag to 1.
 */
void FetchPhase(Fetch *fetch) {
   fetch->outBucket->instruction = **(fetch->pc);
   (*(fetch->pc))++;
   fetch->count++;
   fetch->outBucket->ready = 1;
}
