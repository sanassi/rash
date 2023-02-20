#include "expansion.h"
#include <ctype.h>
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
    return c == '\'' || c == '"';
}

bool is_matching_quote(char c, char quote)
{
    return (quote == '\'' || quote == '"') && c == quote;
}

void single_quote_expand(struct expander *exp)
{
    stream_advance(exp->stream);

    while (!is_quote(stream_peek(exp->stream)))
    {
        char cur = stream_advance(exp->stream);
        *exp->current_str = my_str_cat(*exp->current_str, &cur, 1);
    }
    if (is_quote(stream_peek(exp->stream)))
        stream_advance(exp->stream);
}

void expand_aux(struct expander *exp, struct env *env)
{
    char c = stream_peek(exp->stream);

    if (c == EOF)
        return;

    if (c == '\'') // only single quotes for now
    {
        single_quote_expand(exp);
        expand_aux(exp, env);
        return;
    }
    else if (c == '"')
    {
       if (exp->quoting)
           exp->quoting = false;
       else
           exp->quoting = true;

       stream_advance(exp->stream);
       expand_aux(exp, env);
       return;
    }
    else if (c == '\\')
    {
        stream_advance(exp->stream);
        char cur = stream_advance(exp->stream);
        *exp->current_str = my_str_cat(*exp->current_str, &cur, 1);
        expand_aux(exp, env);
        return;
    }
    else if (c == '$')
    {
        exp->expanding = true;
        stream_advance(exp->stream);

        if (stream_peek(exp->stream) == '{')
        {
            /*TODO : implement :\ */
        }
        else if (stream_peek(exp->stream) == '(')
        {
            /*TODO : implement .. \: */
        }
        else /*simple var subsitution : $toto */
        {
            char *to_replace = NULL;
            char cur;
            while ((cur = stream_peek(exp->stream)) != EOF)
            {
                if (isspace(cur) || is_quote(cur) || cur == '$')
                    break;
                to_replace = my_str_cat(to_replace, &cur, 1);
                stream_advance(exp->stream);
            }

            char *value = env_get_variable(env, to_replace);
            if (value)
                *exp->current_str = 
                    my_str_cat(*exp->current_str, value, strlen(value) + 1);
            free(value);
            free(to_replace);

            expand_aux(exp, env);
            return;
        }
    }
    else
    {
        *exp->current_str = my_str_cat(*exp->current_str, &c, 1);
        stream_advance(exp->stream);
        expand_aux(exp, env);
        return;
    }
}

struct vector *expand(char *str, struct env *env)
{
    struct vector *res = vector_new();
    struct stream *stream = stream_open_string(str);
    struct expander *exp = expander_init();

    exp->stream = stream;

    expand_aux(exp, env);

    if (*exp->current_str)
        vector_append(&res, strdup(*exp->current_str),
                strlen(*exp->current_str) + 1);

    expander_free(exp);

    return res;
}
