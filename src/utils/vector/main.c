#include <stdlib.h>
#include <string.h>

#include "vector.h"

struct trash
{
    int hehe;
    char *str;
};

int main()
{
    struct vector *v = vector_new();

    /*
    struct trash *trash1 = calloc(1, sizeof(struct trash));
    struct trash *trash2 = calloc(1, sizeof(struct trash));
    struct trash *trash3 = calloc(1, sizeof(struct trash));

    vector_append(&v, trash1, sizeof(struct trash *));
    vector_append(&v, trash2, sizeof(struct trash *));
    vector_append(&v, trash3, sizeof(struct trash *));
    */

    for (size_t i = 0; i < 10; i++)
    {
        struct trash *t = calloc(1, sizeof(struct trash));
        vector_append(&v, t, sizeof(t));
    }

    vector_print(v);
    vector_free(v);

    return 0;
}
