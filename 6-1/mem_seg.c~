#include <stdio.h>
#include <stdlib.h>
char globBuf[65536];
int primes[] = { 2, 3, 5, 7 };
static int
square(int x)
{
int result;
result = x * x;
return result;
/* Uninitialized data segment */
/* Initialized data segment */
/* Allocated in frame for square() */
/* Allocated in frame for square() */
/* Return value passed via register */
}
static void
doCalc(int val)
/* Allocated in frame for doCalc() */
{
printf("The square of %d is %d\n", val, square(val));
if (val < 1000) {
int t;
/* Allocated in frame for doCalc() */
t = val * val * val;
printf("The cube of %d is %d\n", val, t);
}
}
int
main(int argc, char *argv[])
{
static int key = 9973;
static char mbuf[10240000];
char *p;
p = malloc(1024);
/* Allocated in frame for main() */
/* Initialized data segment */
/* Uninitialized data segment */
/* Allocated in frame for main() */
/* Points to memory in heap segment */
doCalc(key);
exit(EXIT_SUCCESS);
}
