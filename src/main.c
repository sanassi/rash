#include "repl/repl.h"

int main(int argc, char *argv[])
{
    if (!argv)
        return 1;

    struct program_args *args = parse_cmd_line_args(argc, argv);
    struct env *env = env_init();

    if (args->file)
        env_set_special_variables(env, argc - 1, argv + 1);
    else if (args->string)
        env_set_special_variables(env, argc - 3, argv + 3);
    else
        env_set_special_variables(env, 0, NULL);

    int res = rash_repl(args, env);

    env_free(env);
    args_free(args);

    return res;
}
