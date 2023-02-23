#ifndef EXPANSION_H
#define EXPANSION_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../env/env.h"
#include "../stream/stream.h"
#include "../utils/utils.h"
#include "../utils/vector/vector.h"

struct expander
{
    struct stream *stream;
    struct vector *output;

    bool quoting;
    bool expanding;

    char **current_str;
};

struct vector *expand(char *str, struct env *env);

#endif /* ! EXPANSION_H */
