#ifndef EXECUTION_H
#define EXECUTION_H

#include "../utils/utils.h"
#include "../parser/ast.h"
#include "builtins/bool.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>

#define BUILTIN_ERR 127

typedef int (*run_type)(struct ast *ast);
int run_ast(struct ast *ast);

#endif /* ! EXECUTION_H */
