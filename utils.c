#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

/* Defintions */
char peekch(FILE *stream)
{
    char c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

// Boolean of percent change of being true
bool chancepercent(int percent)
{
    return rand() % 100 <= percent;
}

// Random integer in range [-n, n]
int plusminus(int n) 
{ 
    return n - (rand() % (2 * n + 1));
}
