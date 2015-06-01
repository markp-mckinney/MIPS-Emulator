#include <stdlib.h>
#include <stdio.h>

int iLog2(int x) {
   int i;

   for (i = 32; i; i--) {
      if (x & (1 << (i - 1))) {
         return i;
      }
   }
   return 0;
}

/*	Cache emulation - statistics generation */
/*	Generated for CSC 315 Lab 5 */

int hits;
int misses;
int readCount;
int writeCount;

int size;   //the number of words total stored in cache
int ass;    //the associativity of the cache

int idxMask;   //mask this with the pointer to determine the index in cache

typedef struct {
   int tag;
   int valid;
   //int data; If we were actually implemting a cache, we'd use is to represent the data stored in cache, but we're just trying to determine *whether or not* the cache would have contained the memory we are trying to access.
} CacheEntry;

CacheEntry *cache;

void init_cache(int cacheSize, int assoc) {
   hits = misses = readCount = writeCount = 0;

   ass = assoc;
   size = cacheSize;

   cache = calloc(cacheSize, sizeof(CacheEntry));

   srand(0);

   idxMask = (1 << iLog2(cacheSize / assoc)) - 1;
}

/**
 * Put something in cache.  We will use the following policy:
 *  * if there are any open spaces in a spot in cache, we'll just put it there
 *  * if not, evict an item from cache randomly
 */
int evict_and_replace(int p) {
   int assoc;
   int index = (p & idxMask) * ass;
   int i;

   for (i = 0; i < ass; i++) {
      if (cache[index + i].valid == 0) {
         cache[index + i].valid = 1;
         cache[index + i].tag = p;
         return i;
      }
   }

   i = rand() % ass;
   cache[index + i].tag = p;
   return i;
}

void mem_read(int *mp)
{
   int p = *((int *) ((void *) &mp));
   int index = (p & idxMask) * ass;
   int i;
   int wordFound = 0;

//   printf("Memory read from location %p: ", mp);

   for (i = 0; i < ass; i++) {
      if (cache[index + i].tag == p && cache[index + i].valid) {
//         printf("it's a hit!  assoc %d\n", i + 1);
         hits++;
         wordFound = 1;
         break;
      }
   }

   if (!wordFound) {
      misses++;
      i = evict_and_replace(p);
      //the address was not in cache... need to evict someone and put him there
//      printf("it's a miss :( adding to assoc %d\n", i);
   }

   readCount++;
}

void mem_write(int *mp)
{
   int p = *((int *) ((void *) &mp));
   int index = (p & idxMask) * ass;
   int i;
   int wordFound = 0;

//   printf("Memory write to location %p: ", mp);

   for (i = 0; i < ass; i++) {
      if (cache[index + i].tag == p && cache[index + i].valid) {
//         printf("it's a hit!  assoc %d\n", i + 1);
         hits++;
         wordFound = 1;
         break;
      }
   }

   if (!wordFound) {
      misses++;
      i = evict_and_replace(p);
      //the address was not in cache... need to evict someone and put him there
//      printf("it's a miss :( adding to assoc %d\n", i);
   }

   writeCount++;
}


int main(int argc, char **argv)
{
   int a[10][10], b[10][10], mult[10][10], r1, c1, r2, c2, i, j, k;

   int *mp1, *mp2, *mp3;

   int cacheSize, associativity;

   if (argc != 3) {
      printf("usage: ./a.out <cacheSize> <associativity>\n");
      return 1;
   }

   cacheSize = atoi(argv[1]);
   associativity = atoi(argv[2]);
   init_cache(cacheSize, associativity);

   printf("Size of pointer is: %d\n\n", sizeof(mp1));

   printf("Enter rows and column for first matrix: ");
   scanf("%d%d", &r1, &c1);
   printf("Enter rows and column for second matrix: ");
   scanf("%d%d",&r2, &c2);

   /* If colum of first matrix in not equal to row of second matrix, asking user to enter the size of matrix again. */
   while (c1!=r2)
   {
      printf("Error! column of first matrix not equal to row of second.\n");
      printf("Enter rows and column for first matrix: ");
      scanf("%d%d", &r1, &c1);
      printf("Enter rows and column for second matrix: ");
      scanf("%d%d",&r2, &c2);
   }

   /* Storing elements of first matrix. */
   printf("\nEnter elements of matrix 1:\n");
   for(i=0; i<r1; ++i)
      for(j=0; j<c1; ++j)
      {
         //        printf("Enter elements a%d%d: ",i+1,j+1);
         //        scanf("%d",&a[i][j]);
         a[i][j] = i+j; // build sample data

      }

   /* Storing elements of second matrix. */
   printf("\nEnter elements of matrix 2:\n");
   for(i=0; i<r2; ++i)
      for(j=0; j<c2; ++j)
      {
         //        printf("Enter elements b%d%d: ",i+1,j+1);
         //        scanf("%d",&b[i][j]);

         b[i][j] = 10 + i + j;
      }

   /* Initializing elements of matrix mult to 0.*/
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
      {
         mult[i][j]=0;
      }

   /* Multiplying matrix a and b and storing in array mult. */
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
         for(k=0; k<c1; ++k)
         {

            mp1 = &mult[i][j];
            mp2 = &a[i][k];
            mp3 = &b[k][j];   


            mem_read(mp1);
            mem_read(mp2);
            mem_read(mp3);
            mult[i][j]+=a[i][k]*b[k][j];
            mem_write(mp1); 
         }

   /* Displaying the multiplication of two matrix. */
   printf("\nOutput Matrix:\n");
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
      {
      printf("%d  ",mult[i][j]);
      if(j==c2-1)
         printf("\n\n");
      }

   printf("total hits: %d total misses: %d reads: %d writes: %d\n", hits, misses, readCount, writeCount);
   return 0;
}
