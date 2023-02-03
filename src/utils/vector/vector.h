#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <err.h>

struct node
{
    void *data;
};

struct vector
{
    struct node **nodes;

    size_t size;
    size_t capacity;

    size_t elt_size;
};

struct vector *vector_new();
bool vector_append(struct vector **v, void *elt, size_t elt_size);
void vector_print(struct vector *v);
void vector_free(struct vector *v);
void *vector_get_at(struct vector *v, size_t index);
bool check_capacity(struct vector *v);
char **vector_convert_str_arr(struct vector *v, bool add_null);
#endif /* ! VECTOR_H */
