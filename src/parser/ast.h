#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "../utils/utils.h"

enum ast_type
{
    AST_IF,
    AST_SIMPLE_CMD,
    AST_CMD_LIST,
    AST_ELSE,
    AST_WHILE,
    AST_REDIR,
    AST_UNTIL,
    AST_FOR,
    AST_NEG,
    AST_CMD,
    AST_PIPELINE,
    AST_PIPE,

    AST_AND_OR,
    AST_AND,
    AST_OR,
    AST_ASSIGN,
    AST_FUNC,
    AST_SUBSHELL,
    AST_CASE,
};

struct ast
{
    enum ast_type type;
};

struct ast_simple_cmd
{
    struct ast base;
    struct vector *args;
};

struct ast_cmd_list
{
    struct ast base;
    struct vector *commands;
};

struct ast_if
{
    struct ast base;

    struct ast *condition;
    struct ast *body;

    struct ast *else_body;
};

typedef void (*free_type)(struct ast *ast);
typedef void (*print_type)(struct ast *ast);

#endif /* ! AST_H */
