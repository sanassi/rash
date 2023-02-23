#include "args.h"
#include <bits/getopt_core.h>
#include <err.h>

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
       int c = getopt_long(argc, argv, "pdc:", long_option, &option_index);
       if (c == -1)
           break;

       switch(c)
       {
        case 'p':
            p_args->pretty = true;
            break;
        case 'd':
            p_args->debug = true;
            break;
        case 'c':
            p_args->string = true;
            p_args->str_input = strdup(optarg);
            break;
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
