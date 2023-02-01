#ifndef AST_FREE_H
#define AST_FREE_H

#include <err.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

void free_ast(struct ast *ast);

#endif /* ! AST_FREE_H */
