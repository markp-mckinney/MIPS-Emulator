/**
 * Just some high level C code to make sure we actually understand the format
 */

#include <stdio.h>

#define EXPONENT_MASK   0x7F800000
#define EXP_SHIFT       23
#define SIGN_MASK       0x80000000
#define MAGNITUDE_MASK  0x007FFFFF

float packFloat(int sign, int exponent, int magnitude) {
   int res = (sign ? 1 : 0) << 31;
   res |= ((exponent + 127) << EXP_SHIFT) & EXPONENT_MASK;
   res |= magnitude & MAGNITUDE_MASK;
   printf("packing float: 0x%X (%d)\n", res, res);
   return *((float *) ((void *) &res)); 
}

void unpackFloat(float aV, int *sign, int *exponent, int *magnitude) {
   printf("0x%X\n", aV);
   int a = *((int *) ((void *) &aV));
   printf("0x%X\n", a);
   *sign = a & SIGN_MASK;
   *exponent = ((((int) a) & EXPONENT_MASK) >> EXP_SHIFT) - 127;
   *magnitude = (a & MAGNITUDE_MASK) + (MAGNITUDE_MASK + 1);

   printf(" unpacking float: 0x%X (%f)  sign: 0x%X, exponent: %d, magnitude: %d\n", (int) a,
         aV, *sign, *exponent, *magnitude);
}

void normalizeFloat(int *exponent, int *magnitude) {
   printf("exp: %d, mag: %d\n", *exponent, *magnitude);
   while (*magnitude & !MAGNITUDE_MASK == 0) {
      *magnitude <<= 1;
      (*exponent)--;
      printf("exp: %d, mag: %d\n", *exponent, *magnitude);
   }
   while (*magnitude & !((MAGNITUDE_MASK << 1) + 1)) {
      *magnitude >>= 1;
      (*exponent)++;
      printf("exp: %d, mag: %d\n", *exponent, *magnitude);
   }
}

float fSum(float a, float b) {
   int aSign, aExp, aMag;
   int bSign, bExp, bMag;

   unpackFloat(a, &aSign, &aExp, &aMag);
   unpackFloat(b, &bSign, &bExp, &bMag);

   if (aExp < bExp)
      return fSum(b, a);

   while (bExp < aExp) {
      bExp++;
      bMag >>= 1;
   }
   printf("bExp: exp: 0x%X (%d) mag: 0x%x (%d)\n", 
         bExp, bExp, bMag, bMag);
   if (aSign)
      aMag = -aMag;
   if (bSign)
      bMag = -bMag;

   int cSign = 0, cExp = bExp, cMag;

   cMag = aMag + bMag;
   if (cMag < 0) {
      cSign = 1;
      cMag = -cMag;
   }

   normalizeFloat(&cExp, &cMag);
   return packFloat(cSign, cExp, cMag);
}

float fDiff(float a, float b) {
}

float fProd(float a, float b) {
}

int main(void) {
   float a, b;

   scanf(" %f", &a);
   scanf(" %f", &b);

   printf("sum: %f\n", fSum(a, b));
   printf("diff: %f\n", fDiff(a, b));
   printf("product: %f\n", fProd(a, b));

   return 0;
}
