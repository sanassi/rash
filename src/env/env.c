#include "env.h"
#include <stdlib.h>

struct env *env_init(void)
{
    struct env *env = calloc(1, sizeof(struct env));
    env->variables = hash_map_init(100);
    //env_add_variable(env, "0", "-rash");
    return env;
}

void env_set_special_variables(struct env *env, int argc, char *argv[])
{
    if (!env)
        return;

    char *argc_str = my_itoa(argc);
    env_add_variable(env, "#", argc_str);
    free(argc_str);

    env_add_variable(env, "?", "0");

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
