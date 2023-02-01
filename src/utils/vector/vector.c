#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vector *vector_new()
{
    struct vector *v = calloc(1, sizeof(struct vector));
    v->capacity = 1;
    v->nodes = NULL;
    return v;
}

static bool vector_dub_capacity(struct vector *v)
{
    if (!v)
        return false;
    size_t new_size = 2 * v->capacity;

    struct node **tmp = realloc(v->nodes, new_size * sizeof(struct node *));
    if (!tmp)
        return false;

    v->capacity *= 2;
    v->nodes = tmp;

    return true;
}

bool vector_append(struct vector **v, void *elt, size_t elt_size)
{
    (void) elt_size;

    if (!(*v))
        *v = vector_new();
    struct vector *res = *v;

    if (res->size + 1 == res->capacity)
    {
        bool dub_err = vector_dub_capacity(res);
        if (!dub_err)
            return false;
    }

    res->size += 1;
    size_t size = res->size;
    res->nodes[size - 1] = calloc(1, sizeof(struct node));

    //res->nodes[size - 1]->data = calloc(1, elt_size);
    res->nodes[size - 1]->data = elt;
    //memmove(res->nodes[size - 1]->data, elt, elt_size);

    return true;
}

void vector_free(struct vector *v)
{
    if (!v)
        return;

    for (size_t i = 0; i < v->size; i++)
    {
        //free(v->nodes[i]->data);
        free(v->nodes[i]);
    }

    free(v->nodes);
    free(v);
}

void vector_print(struct vector *v)
{
    printf("size = %lu\ncap = %lu\n", v->size, v->capacity);
    for (size_t i = 0; i < v ->size; i++)
    {
        void *ptr = v->nodes[i]->data;
        printf("%p\n", ptr);
    }
}
