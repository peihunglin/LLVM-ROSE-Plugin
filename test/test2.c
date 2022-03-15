#include <stdlib.h>
 
int main() {
   int* a = malloc(sizeof(int) * 2);
//   a[0] = 22;
//   a[1] = 100;
/*
   int b;
   int *p=a;
   int *q=a; // q is now an alias of p
   int* r=0;
   if(rand()) // make sure both branches are analyzed
        r=a;
    else
        r=&b;
    *r=100; // r points to a or b
    q = r;
*/
  return 0;
}
