#include <string.h>

#include "vector.h"

//vec_(char);

vec_char *vec_char_create() 
{
    vec_char *ptr = (vec_char*)malloc(sizeof(vec_char*));
    ptr->size = 1;
    ptr->size_actual = 0;
    ptr->data = (char *)malloc(sizeof(char));
    return ptr;
}

void vec_char_delete(vec_char *vec) 
{
    free(vec->data);
    free(vec);
}

void vec_char_grow(vec_char *vec) 
{
    vec->size *= 2;
    vec->data = realloc(vec->data, sizeof(char) * vec->size);
}

void vec_char_realloc(vec_char *vec, size_t newsize) 
{
    vec->size = newsize + 1;
    vec->size_actual = newsize;
    vec->data = realloc(vec->data, sizeof(char) * newsize + 1);
    vec->data[vec->size_actual] = 0;
}

void vec_char_push(vec_char *vec, char val) 
{
    if (vec->size == vec->size_actual + 1)
        vec_char_grow(vec);
    vec->data[vec->size_actual++] = val;
    vec->data[vec->size_actual] = 0;
}

void vec_char_splice(vec_char *dest, vec_char *src, size_t start) 
{
    if (dest->size_actual == 0 && src->size_actual == 0)
        return;
    if (start >= dest->size)
        start = dest->size_actual;
    int need_to_shift = dest->size_actual - start;
    size_t newsize = dest->size_actual + src->size_actual;
    vec_char_realloc(dest, newsize);
    for (int i = need_to_shift - 1; i >= 0; i--)
        dest->data[i + start + src->size_actual] = dest->data[i + start];
    for (size_t i = 0; i < src->size_actual; i++)
        dest->data[i + start] = src->data[i];
}

void vec_char_remove(vec_char* vec, size_t index, size_t delcount) 
{
    if (index + delcount > vec->size_actual)
        delcount = vec->size_actual - index;
    int newsize = vec->size_actual - delcount;
    for (size_t i = index; i < vec->size_actual; i++)
        vec->data[i] = vec->data[i + delcount];
    vec_char_realloc(vec, newsize);
}

vec_char* vec_char_from(char* src, size_t size) 
{
    vec_char* res = vec_char_create();

    // Grow for src size + 1
    while (res->size <= size)
        vec_char_grow(res);
    for (size_t i = 0; i < size; i++)
        vec_char_push(res, src[i]);
    return res;
}

vec_char* vec_char_from2(char* src) 
{
    size_t size = strlen(src);
    vec_char* res = vec_char_create();
    
    // Empty string
    if (!size)
    {
        res->data[size] = 0;
        return res;
    }
    else
    {
        // Grow for src size + 1
        while (res->size <= size)
            vec_char_grow(res);
        for (size_t i = 0; i < size; i++)
            vec_char_push(res, src[i]);
        return res;
    }
}

