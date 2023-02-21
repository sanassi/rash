#include "env.h"
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 1024

struct env *env_init(void)
{
    struct env *env = calloc(1, sizeof(struct env));
    env->variables = hash_map_init(100);
    return env;
}

void env_set_special_variables(struct env *env, int argc, char *argv[])
{
    if (!env)
        return;
    
    env->argc = (size_t) argc;

    char *argc_str = my_itoa(argc);
    env_add_variable(env, "#", argc_str);
    free(argc_str);

    env_add_variable(env, "?", "0");

    int pid = getgid();
    char *pid_to_str = my_itoa(pid);
    env_add_variable(env, "$", pid_to_str);
    free(pid_to_str);

    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    env_add_variable(env, "PWD", cwd);

    env_add_variable(env, "IFS", "\n \t");

    for (int i = 0; i < argc; i++)
    {
        char *index_to_str = my_itoa(i);
        env_add_variable(env, index_to_str, argv[i]);
        free(index_to_str);
    }
}

void env_free(struct env *env)
{
    if (!env)
        return;
    hash_map_free(env->variables);
    free(env);
}

void env_add_variable(struct env *env, const char *id, char *value)
{
    if (!env)
        return;
    bool updated = true;
    hash_map_insert(env->variables, id, value, &updated);
}

char *env_get_variable(struct env *env, const char *id)
{
    return hash_map_get(env->variables, id);
}
