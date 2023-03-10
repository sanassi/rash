#include "echo.h"

#include <bits/getopt_core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(char *str, bool interpret)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] != '\\')
        {
            printf("%c", str[i]);
            continue;
        }

        if (!interpret)
        {
            printf("%c", str[i]);
            continue;
        }

        switch (str[i + 1])
        {
        case 'n':
            printf("\n");
            i += 1;
            break;
        case 't':
            i += 1;
            printf("\t");
            break;
        case '\\':
            i += 1;
            printf("\\");
            break;
        default:
            printf("%c", str[i]);
            break;
        }
    }
}

int echo(struct vector *args, struct env *env)
{
    (void)env; /* needs to be removed */
    char **args_str = vector_convert_str_arr(args, false);

    int c;
    bool interpret;
    bool print_newline = true;

    for (size_t i = 0; i < args->size; i++)
    {
        if (!args_str[i])
            printf("NULL\n");
    }

    optind = 1;
    opterr = 0;

    bool stop = false;

    while ((c = getopt(args->size, args_str, "neE")) != -1)
    {
        switch (c)
        {
        case 'n':
            print_newline = false;
            break;
        case 'e':
            interpret = true;
            break;
        case 'E':
            interpret = false;
            break;
        case '?':
            optind -= 1;
            stop = true;
            break;
        default:
            break;
        }
        if (stop)
            break;
    }

    for (size_t i = optind; i < args->size; i++)
    {
        print(args_str[i], interpret);
        if (i != args->size - 1)
            printf(" ");
    }

    if (print_newline)
        printf("\n");

    for (size_t i = 0; i < args->size; i++)
        free(args_str[i]);
    free(args_str);

    optind = 1;

    return 0;
}

/*
int main(int argc, char *argv[])
{
    struct vector *v = vector_new();
    for (int i = 0; i < argc; i++)
        vector_append(&v, argv[i], strlen(argv[i]));

    echo(v);

    vector_free(v);

    return 0;
}
*/
