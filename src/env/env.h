#ifndef ENV_H
#define ENV_H

#include "../utils/hash_map/hash_map.h"
#include "../utils/vector/vector.h"
#include "../utils/utils.h"

struct env
{
    struct hash_map *variables;
};

struct env *env_init(void);
void env_free(struct env *env);
void env_add_variable(struct env *env, const char *id, char *value);
char *env_get_variable(struct env *env, const char *id);
void env_set_special_variables(struct env *env, int argc, char *argv[]);
#endif /* ! ENV_H */
