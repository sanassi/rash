#include "env.h"
#include <stdlib.h>

struct env *env_init(void)
{
    struct env *env = calloc(1, sizeof(struct env));
    env->variables = hash_map_init(100);
    return env;
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
