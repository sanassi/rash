#ifndef ARGS_H
#define ARGS_H

//#include <bits/getopt_core.h>
//#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../env/env.h"
#include "../utils/utils.h"

struct program_args
{
    bool string;
    bool file;
    bool input;

    bool pretty;

    bool debug;

    char *file_path;
    char *str_input;
    char **args;
    size_t nb_args;
};

struct program_args *parse_cmd_line_args(int argc, char *argv[]);
void print_args(struct program_args *args);
void args_free(struct program_args *args);

#endif /* ! ARGS_H */
