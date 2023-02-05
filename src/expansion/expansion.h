#ifndef EXPANSION_H
#define EXPANSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../utils/utils.h"
#include "../utils/vector/vector.h"
#include "../stream/stream.h"

struct expander
{
    struct stream *stream;
    struct vector *output;

    char **current_str;
};

struct vector *expand(char *str);

#endif /* ! EXPANSION_H */
