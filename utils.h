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

/* Buffer for storing lexemes */
#define BUF_SIZE (24 * 10)

typedef enum { false, true } bool;

/* Declarations */
// Peek next char without extracting from stream
extern char peekch(FILE *stream);
// Random integer in range [-n, n]
extern int plusminus(int n);
// Boolean of percent chance of being true
extern bool chancepercent(int percent);

#endif /* UTILS_H */
