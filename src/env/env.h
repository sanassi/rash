#ifndef ENV_H
#define ENV_H

#include <sys/types.h>
#include <unistd.h>

#include "../utils/hash_map/hash_map.h"
#include "../utils/utils.h"
#include "../utils/vector/vector.h"

struct env
{
    size_t argc;
    struct hash_map *variables;

    int exit_value;
    int last_cmd_value;

    size_t nb_nested_loops;
    size_t nb_continue;
    size_t nb_break;

    struct vector *functions;

    bool isolated;

    struct env *enclosing;
};

struct env *env_init(void);
void env_free(struct env *env);
void env_add_variable(struct env *env, const char *id, char *value);
char *env_get_variable(struct env *env, const char *id);
void env_set_special_variables(struct env *env, int argc, char *argv[]);
struct ast_func *env_get_function(struct env *env, char *name);
bool env_add_function(struct env *env, struct ast_func *func);

#endif /* ! ENV_H */
