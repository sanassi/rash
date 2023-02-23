#include "dot.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../args/args.h"
#include "../../repl/repl.h"

int dot(struct vector *args, struct env *env)
{
    char *file = strdup((char *)vector_get_at(args, 1));
    struct program_args *p_args = calloc(1, sizeof(struct program_args));
    p_args->file = true;
    p_args->pretty = false;
    p_args->file_path = file;

    int res = rash_repl(p_args, env);

    args_free(p_args);

    return res;
}
