#include "execution/builtins/bool.h"
#include "parser/ast_free.h"
#include "parser/ast_print.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "stream/stream.h"
#include "execution/execution.h"
#include <bits/getopt_core.h>
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "env/env.h"
#include "utils/utils.h"

struct program_args
{
    bool string;
    bool file;
    bool input;

    bool pretty;

    char *file_path;
    char *str_input;
    char **args;
    size_t nb_args;
};

struct program_args *parse_cmd_line_args(int argc, char *argv[])
{
   struct program_args *p_args = calloc(1, sizeof(struct program_args)); 

   if (argc == 1)
   {
       p_args->input = true;
       return p_args;
   }

   while (true)
   {
       static struct option long_option[] = 
       {
           {"pretty-print", no_argument, 0, 'p'},
           {0, 0, 0, 0},
       };

       int option_index = 0;
       int c = getopt_long(argc, argv, "pc:", long_option, &option_index);
       if (c == -1)
           break;

       switch(c)
       {
        case 'p':
            p_args->pretty = true;
            break;
        case 'c':
            p_args->string = true;
            p_args->str_input = strdup(optarg);
        case '?':
            break;
        default:
            errx(2, "invalid option");
           break;
       }
   }

   if (optind < argc)
   {
       if (!p_args->string)
       {
           p_args->file = true;
           p_args->file_path = strdup(argv[optind]);
           optind += 1;
       }

       p_args->args = argv + optind;
       p_args->nb_args = argc - optind;
   }


   return p_args;
}

void print_args(struct program_args *args)
{
    if (args->pretty)
        printf("pretty print\n");
    if (args->string)
        printf("-c %s\n", args->str_input);
    if (args->input)
        printf("stdin\n");
    if (args->file)
        printf("file %s\n", args->file_path);
    for (size_t i = 0; i < args->nb_args; i++)
        printf("%s ", args->args[i]);
    printf("\n");
}

void args_free(struct program_args *args)
{
    if (args->string)
        free(args->str_input);
    if (args->file)
        free(args->file_path);
    free(args);
}

struct stream *stream_open(struct program_args *args)
{
    if (args->file)
        return stream_open_file(args->file_path);
    if (args->string)
        return stream_open_string(args->str_input);
    if (args->input)
        return stream_open_stdin();
    errx(2, "invalid stream options");
}

int main(int argc, char *argv[])
{
    if (!argv)
        return 1;

    struct program_args *args = parse_cmd_line_args(argc, argv);

    struct env *env = env_init();
    struct stream *stream = stream_open(args);
    struct lexer *l = lexer_init();

    l->stream = stream;

    int run_status = true_builtin();
    int parse_status = true_builtin();

    if (args->file)
        env_set_special_variables(env, argc - 1, argv + 1);
    else if (args->string)
        env_set_special_variables(env, argc - 3, argv + 3);
    else
        env_set_special_variables(env, 0, NULL);

    while (true)
    {
        struct parser *p = parser_init();
        p->lexer = l;
        struct ast *root = NULL;

        parse_status = parse_input(p, &root);

        if (parse_status == PARSER_ERROR && !args->input)
            errx(parse_status, "grammar error");

        if (args->pretty)
            print_ast(root);

        if (root)
            run_status = run_ast(root, env);

        if (run_status == BUILTIN_ERR && !args->input)
            errx(run_status, "execution error");

        fflush(stream->fp);

        bool done = p->is_at_end;

        free_ast(root);
        parser_free(p);

        if (done)
            break;
    }

    for (size_t i = 0; i < l->tokens->size; i++)
        token_free(l->tokens->nodes[i]->data);

    env_free(env);
    vector_free(l->tokens);
    lexer_free(l);
    stream_free(stream);
    args_free(args);
    return run_status;
}
