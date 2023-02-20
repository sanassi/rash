#include "expansion.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    struct env *env = env_init();
    env_add_variable(env, "toto", "poop");
    char *str = "echo $toto$heheh tehwe";
    printf("%s\n", str);
    struct vector *res = expand(str, env);

    printf("%s\n", (char *) vector_get_at(res, 0));

    free(vector_get_at(res, 0));
    vector_free(res);
    env_free(env);

    return 0;
}
