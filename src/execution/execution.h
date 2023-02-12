#ifndef EXECUTION_H
#define EXECUTION_H

#include "../utils/utils.h"
#include "../parser/ast.h"
#include "builtins/bool.h"
#include "builtins/echo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include "../expansion/expansion.h"

#define BUILTIN_ERR 127
#define REDIR_BUILTIN_ERR 125

typedef int (*run_type)(struct ast *ast);
typedef int (*builtin_run)(struct vector *args);
int run_ast(struct ast *ast);

#endif /* ! EXECUTION_H */
