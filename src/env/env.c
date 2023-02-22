#include "env.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../parser/ast.h"
#include "../parser/ast_free.h"

#define PATH_MAX 1024

/**
 * Initialize an empty environment
 */
struct env *env_init(void)
{
    struct env *env = calloc(1, sizeof(struct env));
    env->variables = hash_map_init(100);
    return env;
}

/**
 * Set the environement special variables from
 * the program arguments.
 */
void env_set_special_variables(struct env *env, int argc, char *argv[])
{
    if (!env)
        return;
    
    env->argc = (size_t) argc;

    // $0 is not included in $#
    char *argc_str;
    if (argc == 0)
        argc_str = my_itoa(0);
    else
        argc_str = my_itoa(argc - 1);

    env_add_variable(env, "#", argc_str);
    free(argc_str);

    env_add_variable(env, "?", "0");

    int pid = getgid();
    char *pid_to_str = my_itoa(pid);
    env_add_variable(env, "$", pid_to_str);
    free(pid_to_str);

    /*
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    env_add_variable(env, "PWD", cwd);
    */

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

    if (env->functions)
    {
        for (size_t i = 0; i < env->functions->size; i++)
        {
            struct ast_func *func = vector_get_at(env->functions, i);
            func->nb_references = 0;
            free_ast((struct ast*) func);
        }
        vector_free(env->functions);
    }

    hash_map_free(env->variables);
    free(env);
}

/**
 * Add a variable to the current environment.
 * If the environment has a parent, and is not isolated,
 * add the variable to the parent.
 * Otherwise, add the variable to the current environment.
 */

void env_add_variable(struct env *env, const char *id, char *value)
{
    if (!env)
        return;

    if (env->isolated)
    {
        bool updated = true;
        hash_map_insert(env->variables, id, value, &updated);
        return;
    }

    /* fond the global env (can even fuse the 2 conditions) */
    if (!env->enclosing)     
    {
        bool updated = true;
        hash_map_insert(env->variables, id, value, &updated);
        return;
    }
    else
        env_add_variable(env->enclosing, id, value);
}

/**
 * Returns the value of a variable given its id.
 * If the environment does not have a parent environment,
 * return the variable from its hash_map of variables.
 * Otherwise, check if the variable exists in the current environment,
 * if not, check if it exists in its parent.
 */

char *env_get_variable(struct env *env, const char *id)
{
    if (!env->enclosing)
    {
        if (strcmp("PWD", id) == 0)
            return strdup(getenv("PWD"));
        if (strcmp("OLDPWD", id) == 0)
            return strdup(getenv("OLDPWD"));

        return hash_map_get(env->variables, id);
    }
    else
    {
        char *test = hash_map_get(env->variables, id);
        if (test)
            return test;

        return env_get_variable(env->enclosing, id);
    }
}

struct ast_func *env_get_function(struct env *env, char *name)
{
    if (!env->enclosing)
    {
        if (!env->functions)
            return NULL;
        for (size_t i = 0; i < env->functions->size; i++)
        {
            struct ast_func *func = vector_get_at(env->functions, i);
            if (strcmp(func->name, name) == 0)
                return func;
        }
        return NULL;
    }
    else
        return env_get_function(env->enclosing, name);
}

bool env_add_function(struct env *env, struct ast_func *func)
{
    if (!env->enclosing || env->isolated)
        return vector_append(&env->functions, func, sizeof(struct ast_func *));
    else
        return env_add_function(env->enclosing, func);
}
