#ifndef DICT_H
#define DICT_H

#include <stdlib.h>
#include <stdio.h>

#include "vector.h"

typedef struct {
    vec_char *key;
    vec_char *value;
} dict_node;

typedef struct {
    dict_node **nodes;
    size_t size;
    size_t size_actual;
} dict;

extern dict *dict_create();

extern void dict_grow(dict *d);

extern void dict_insert(dict *d, vec_char *key, vec_char *val);

extern vec_char *dict_get(dict *d, vec_char *key);

extern void dict_delete(dict *d);

#endif /* DICT_H */
