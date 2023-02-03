#include "echo.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(char *str, bool interpret)
{
    for (size_t i = 0; str[i]; i++)
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
                break;
            case 't':
                printf("\t");
                break;
            case '\\':
                printf("\\\\");
                break;
            default:
                break;
        }
    }
}

int echo(struct vector *args)
{
    char **args_str = vector_convert_str_arr(args, false);

    int c;
    bool interpret;
    bool print_newline = true;

    opterr = 0;

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
            break;
        default:
            break;
        }
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

    return 0;
}

/*
int main(int argc, char *argv[])
{
    struct vector *v = vector_new();
    for (int i = 0 ; i < argc; i++)
        vector_append(&v, argv[i], strlen(argv[i]));

    echo(v);

    vector_free(v);

    return 0;
}
*/
