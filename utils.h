#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c""%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x8000 ? '1' : '0'), \
  (byte & 0x4000 ? '1' : '0'), \
  (byte & 0x2000 ? '1' : '0'), \
  (byte & 0x1000 ? '1' : '0'), \
  (byte & 0x0800 ? '1' : '0'), \
  (byte & 0x0400 ? '1' : '0'), \
  (byte & 0x0200 ? '1' : '0'), \
  (byte & 0x0100 ? '1' : '0'), \
  (byte & 0x0080 ? '1' : '0'), \
  (byte & 0x0040 ? '1' : '0'), \
  (byte & 0x0020 ? '1' : '0'), \
  (byte & 0x0010 ? '1' : '0'), \
  (byte & 0x0008 ? '1' : '0'), \
  (byte & 0x0004 ? '1' : '0'), \
  (byte & 0x0002 ? '1' : '0'), \
  (byte & 0x0001 ? '1' : '0') 

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))

typedef enum { false, true } bool;

/* Declarations */
// Peek next char without extracting from stream
char peekch(FILE *stream);
// Random integer in range [-n, n]
int plusminus(int n);
// Boolean of percent change of being true
bool chancepercent(int percent);

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

#endif /* UTILS_H */
