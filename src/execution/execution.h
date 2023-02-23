#ifndef EXECUTION_H
#define EXECUTION_H

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../env/env.h"
#include "../expansion/expansion.h"
#include "../parser/ast.h"
#include "../utils/utils.h"
#include "builtins/bool.h"
#include "builtins/cd.h"
#include "builtins/echo.h"

#define BUILTIN_ERR 127
#define REDIR_BUILTIN_ERR 125
#define EXIT_CODE 666

typedef int (*run_type)(struct ast *ast, struct env *env);
typedef int (*builtin_run)(struct vector *args, struct env *env);
int run_ast(struct ast *ast, struct env *env);

#endif /* ! EXECUTION_H */
