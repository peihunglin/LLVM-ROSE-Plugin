#include <stdlib.h>
 
int main() {
   int a;
   int b;
   int *p=&a;
   int *q=&a; // q is now an alias of p
   int* r=0;
   if(rand()) // make sure both branches are analyzed
        r=&a;
    else
        r=&b;
    *r=100; // r points to a or b
}
