#include "cd.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH_MAX 4096

int cd(struct vector *args, struct env *env)
{
    (void)env; /* need to find a way to remove this.. */
    char buffer[PATH_MAX];
    char *cwd = getcwd(buffer, PATH_MAX);
    if (!cwd)
    {
        fprintf(stderr, "Error\n");
        return 1;
    }

    if (args->size == 1 /*!path || *path == '\0' || *path == '\n'*/)
    {
        setenv("OLDPWD", cwd, 1);
        char *home = getenv("HOME");
        if (chdir(home) < 0)
        {
            fprintf(stderr, "cd : error\n");
            return 1;
        }
        setenv("PWD", home, 1);
        return 0;
    }

    char *path = vector_get_at(args, 1);

    if (strcmp("-", path) && !is_dir(path))
    {
        fprintf(stderr, "cd error\n");
        return 1;
    }

    if (!strcmp("-", path) && !is_dir(path) && !file_exists(path))
    {
        char *previous = strdup(getenv("OLDPWD"));

        printf("%s\n", previous);
        setenv("OLDPWD", cwd, 1);
        if (chdir(previous) < 0)
        {
            fprintf(stderr, "cd error\n");
            return 1;
        }
        setenv("PWD", previous, 1);
        free(previous);
        return 1;
    }

    setenv("OLDPWD", cwd, 1);
    if (chdir(path) < 0)
    {
        fprintf(stderr, "cd : directory %s doesn't exist\n", path);
        return 1;
    }

    cwd = getcwd(buffer, PATH_MAX);
    setenv("PWD", cwd, 1);

    return 0;
}
