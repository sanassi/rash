#include "expansion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct expander *expander_init(void)
{
    struct expander *exp = calloc(1, sizeof(struct expander));
    exp->current_str = calloc(1, sizeof(char *));
    return exp;
}

void expander_free(struct expander *exp)
{
    stream_free(exp->stream);
    free(*exp->current_str);
    free(exp->current_str);
    free(exp);
}

bool is_quote(char c)
{
    return c == '\'';
}

void expand_aux(struct expander *exp)
{
    char c = stream_peek(exp->stream);

    if (c == EOF)
        return;

    if (is_quote(c)) // only single quotes for now
    {
        stream_advance(exp->stream);

        while (!is_quote(stream_peek(exp->stream)))
        {
            char cur = stream_advance(exp->stream);
            *exp->current_str = my_str_cat(*exp->current_str, &cur, 1);
        }
        if (is_quote(stream_peek(exp->stream)))
            stream_advance(exp->stream);
        expand_aux(exp);
        return;
    }
    else if (c == '\\')
    {
        stream_advance(exp->stream);
        char cur = stream_advance(exp->stream);
        *exp->current_str = my_str_cat(*exp->current_str, &cur, 1);
        expand_aux(exp);
        return;
    }
    else
    {
        *exp->current_str = my_str_cat(*exp->current_str, &c, 1);
        stream_advance(exp->stream);
        expand_aux(exp);
        return;
    }
}

struct vector *expand(char *str)
{
    struct vector *res = vector_new();
    struct stream *stream = stream_open_string(str);
    struct expander *exp = expander_init();

    exp->stream = stream;

    expand_aux(exp);

    vector_append(&res, strdup(*exp->current_str), 
            strlen(*exp->current_str) + 1);

    expander_free(exp);

    return res;
}
