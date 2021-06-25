#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

//#include "utils.h"

/**************************
 *                        *
 * vector API for vec_(T) *
 *                        *
 **************************
 *
 * PUBLIC:
 *
 * vec_T_create() -> vec_T*
 * Creates a vector of type T.
 * Note: Don't forget to call delete when done!
 *
 * vec_T_from(T *src, int size) -> vec_T*
 * Creates a vector of type T from an array of T's.
 * Note: Don't forget to call 'delete' when done!
 *
 * vec_T_delete(vec_T *vec) -> void
 * Deletes a vector created from 'create' or 'from'.
 *
 * vec_T_push(vec_t *vec, T val) -> void
 * Pushes a value of type T to the end of the vector.
 *
 * vec_T_remove(vec_T *vec, int index, int delcount) -> void
 * Removes 'delcount' values starting at 'index'.
 *
 * vec_T_splice(vec_T *dest, vec_T *src, int start) -> void
 * Splices in all values from 'src' vector to 'dest' at index 'start'.
 *
 *
 * PRIVATE:
 *
 * vec_T_grow(vec_T *vec) -> void
 *
 * vec_T_realloc(vec_T *vec, int len) -> void
 *
 */

//----------------------------------------------------
#define vec_(TYPE)\
    typedef struct {\
        size_t size_actual;\
        size_t size;\
        TYPE *data;\
    } vec_##TYPE;\
    vec_##TYPE *vec_##TYPE##_create() {\
        vec_##TYPE *ptr = (vec_##TYPE*)malloc(sizeof(vec_##TYPE*));\
        ptr->size = 1;\
        ptr->size_actual = 0;\
        ptr->data = (TYPE *)malloc(sizeof(TYPE));\
        return ptr;\
    }\
    void vec_##TYPE##_delete(vec_##TYPE *vec) {\
        free(vec->data);\
        free(vec);\
    }\
    void vec_##TYPE##_grow(vec_##TYPE *vec) {\
        vec->size *= 2;\
        vec->data = realloc(vec->data, sizeof(TYPE) * vec->size);\
    }\
    void vec_##TYPE##_realloc(vec_##TYPE *vec, size_t len) {\
        vec->size = len;\
        vec->size_actual = len;\
        vec->data = realloc(vec->data, sizeof(TYPE) * len);\
    }\
    void vec_##TYPE##_push(vec_##TYPE *vec, TYPE val) {\
        if (vec->size == vec->size_actual) {\
            vec_##TYPE##_grow(vec);\
        }\
        vec->data[vec->size_actual++] = val;\
    }\
    void vec_##TYPE##_splice(vec_##TYPE *dest, vec_##TYPE *src, size_t start) {\
        if (dest->size_actual == 0 && src->size_actual == 0)\
            return;\
        if (start >= dest->size)\
            start = dest->size_actual;\
        int need_to_shift = dest->size_actual - start;\
        size_t newsize = dest->size_actual + src->size_actual;\
        vec_##TYPE##_realloc(dest, newsize);\
        for (int i = need_to_shift - 1; i >= 0; i--)\
            dest->data[i + start + src->size_actual] = dest->data[i + start];\
        for (size_t i = 0; i < src->size_actual; i++)\
            dest->data[i + start] = src->data[i];\
    }\
    void vec_##TYPE##_remove(vec_##TYPE* vec, size_t index, size_t delcount) {\
        if (index + delcount > vec->size_actual)\
            delcount = vec->size_actual - index;\
        int newsize = vec->size_actual - delcount;\
        for (size_t i = index; i < vec->size_actual; i++)\
            vec->data[i] = vec->data[i + delcount];\
        vec_##TYPE##_realloc(vec, newsize);\
    }\
    vec_##TYPE* vec_##TYPE##_from(TYPE* src, size_t size) {\
        vec_##TYPE* res = vec_##TYPE##_create();\
        for (size_t i = 0; i < size; i++)\
            vec_##TYPE##_push(res, src[i]);\
        return res;\
    }

//----------------------------------------------------

vec_(char);

/*
typedef struct {
    int *data;
    int size_actual;
    int size;
} vec_int;

vec_int* vec_int_create()
{
    vec_int* ptr = (vec_int*)malloc(sizeof(vec_int*));
    ptr->size = 1;
    ptr->size_actual = 0;
    ptr->data = (int*)malloc(sizeof(int));
    
    return ptr;
}

vec_char* vec_char_from(char* src, int size)
{
    vec_char* res = vec_char_create();
    for (int i = 0; i < size; i++)
        vec_char_push(res, src[i]);
    return res;
}

void vec_int_delete(vec_int* vec)
{
    free(vec->data);
    free(vec);
}

void vec_int_realloc(vec_int* vec)
{
    vec->size *= 2;
    vec->data = realloc(vec->data, sizeof(int) * vec->size);
}

void vec_int_push(vec_int* vec, int val)
{
    if (vec->size == vec->size_actual) 
    {
        vec_int_realloc(vec);
    }
    vec->data[vec->size_actual++] = val;
}

void vec_char_splice(vec_char *dest, vec_char *src, int start) 
{
    if (dest->size_actual == 0 && src->size_actual == 0)
        return;
    
    if (start >= dest->size)
        start = dest->size_actual;

    int need_to_shift = dest->size_actual - start;
    int newsize = dest->size_actual + src->size_actual;

    vec_char_realloc(dest, newsize);

    for (int i = need_to_shift - 1; i >= 0; i--)
        dest->data[i + start + src->size_actual] = dest->data[i + start];
    for (int i = 0; i < src->size_actual; i++)
        dest->data[i + start] = src->data[i];
}

void vec_char_remove(vec_char* vec, int index, int delcount)
{
    int newsize = vec->size_actual - delcount;

    for (int i = index; i < vec->size_actual; i++)
        vec->data[i] = vec->data[i + delcount];

    vec_char_realloc(vec, newsize);
}



*/

#endif /* VECTOR_H */
