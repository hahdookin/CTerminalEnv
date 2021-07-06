#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"
#include "dict.h"

// typedef struct {
//     vec_char *key;
//     vec_char *value;
// } dict_node;
// 
// typedef struct {
//     dict_node **nodes;
//     size_t size;
//     size_t size_actual;
// } dict;

dict *dict_create()
{
    dict *res;
    res = (dict*)malloc(sizeof(dict));
    res->nodes = (dict_node**)malloc(sizeof(dict_node*));
    res->size = 1;
    res->size_actual = 0;
    return res;
}

void dict_grow(dict *d)
{
    d->size *= 2;
    d->nodes = realloc(d->nodes, sizeof(dict_node*) * d->size);
}

void dict_insert(dict *d, vec_char *key, vec_char *val)
{
    if (d->size == d->size_actual)
        dict_grow(d);

    dict_node *node = malloc(sizeof(dict_node));
    node->key = key;
    node->value = val;

    d->nodes[d->size_actual++] = node;
}

vec_char *dict_get(dict *d, vec_char *key)
{
    for (size_t i = 0; i < d->size_actual; i++)
    {
        if (!strcmp(key->data, d->nodes[i]->key->data))
        {
            return d->nodes[i]->value;
        }
    }
    return NULL;
}

void dict_delete(dict *d)
{
    for (size_t i = 0; i < d->size_actual; i++)
    {
        vec_char_delete(d->nodes[i]->key);
        vec_char_delete(d->nodes[i]->value);
        free(d->nodes[i]);
    }
    free(d);
}


