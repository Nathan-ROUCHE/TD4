#include "stdio.h"

int main(void) {
  int a, b, *p;
   a = 17;
   b = a;
   p = &a;
   b = b + 2;
   *p = a + *p;
   p = p + 2;
   printf("%d", p);
}