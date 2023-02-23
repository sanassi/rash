#include "vector.h"

struct vector *vector_new()
{
    struct vector *v = calloc(1, sizeof(struct vector));
    v->capacity = 1;
    v->size = 0;
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

/**
 * If the vector is full, double its caapacity.
 * Return false if there is an error.
 */
bool check_capacity(struct vector *v)
{
    if (v->size + 1 == v->capacity)
        return vector_dub_capacity(v);
    return false;
}

bool vector_append(struct vector **v, void *elt, size_t elt_size)
{
    (void)elt_size;

    if (!(*v))
        *v = vector_new();
    struct vector *res = *v;

    /*
    if (res->size + 1 == res->capacity)
    {
        bool dub_err = vector_dub_capacity(res);
        if (!dub_err)
            return false;
    }
    */
    check_capacity(res);

    res->size += 1;
    size_t size = res->size;
    res->nodes[size - 1] = calloc(1, sizeof(struct node));

    // res->nodes[size - 1]->data = calloc(1, elt_size);
    res->nodes[size - 1]->data = elt;
    // memmove(res->nodes[size - 1]->data, elt, elt_size);

    return true;
}

void *vector_get_at(struct vector *v, size_t index)
{
    if (index >= v->size)
        errx(1, "vector: out of range exception");

    return v->nodes[index]->data;
}

/**
 * Returns and array of strings from a vector.
 * If add_null is true, add NULL at the end of the array.
 *
 * The data needs to be freed after usage, to prevent memory leaks.
 */
char **vector_convert_str_arr(struct vector *v, bool add_null)
{
    char **res = NULL;
    size_t i = 0;
    for (; i < v->size; i++)
    {
        char *copy = strdup(vector_get_at(v, i));
        char **tmp = realloc(res, (i + 1) * sizeof(char *));
        if (tmp)
            res = tmp;
        res[i] = copy;
    }

    if (add_null)
    {
        char **tmp = realloc(res, (i + 1) * sizeof(char *));
        if (tmp)
            res = tmp;
        res[i] = NULL;
    }

    return res;
}

/**
 * Free the given vector.
 * Does not free the data, only the vector struct.
 */
void vector_free(struct vector *v)
{
    if (!v)
        return;

    for (size_t i = 0; i < v->size; i++)
    {
        // free(v->nodes[i]->data);
        free(v->nodes[i]);
    }

    free(v->nodes);
    free(v);
}

void vector_print(struct vector *v)
{
    printf("size = %lu\ncap = %lu\n", v->size, v->capacity);
    for (size_t i = 0; i < v->size; i++)
    {
        void *ptr = v->nodes[i]->data;
        printf("%p\n", ptr);
    }
}
