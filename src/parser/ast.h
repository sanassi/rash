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

enum redirection_type
{
    REDIR_GREAT, // >
    REDIR_LESS, // <
    REDIR_DGREAT, // >>
    REDIR_GREATAND, // >&
    REDIR_LESSAND, // <&
    REDIR_CLOBBER, // >|
    REDIR_LESSGREAT, // <>
};

struct ast
{
    enum ast_type type;
};

struct ast_simple_cmd
{
    struct ast base;
    struct vector *args;

    struct vector *redir_pref;
    struct vector *redir_suff;
};

struct ast_cmd_list
{
    struct ast base;
    struct vector *commands;
};

struct ast_cmd
{
    struct ast base;
    struct ast *command;
    
    struct vector *redirections;
};

struct ast_if
{
    struct ast base;

    struct ast *condition;
    struct ast *body;

    struct ast *else_body;
};

struct ast_redir
{
    struct ast base;

    char *redir_op;

    bool has_io_number;
    int io_number;

    int start_fd;
    int new_fd;
    enum redirection_type redir_type;

    char *file;
};

struct ast_pipe
{
    struct ast base;

    struct ast *left;
    struct ast *right;
};

struct ast_pipeline
{
    struct ast base;

    struct ast *pipe;
};

struct ast_neg
{
    struct ast base;
    struct ast *pipeline;
};

struct ast_and_or
{
    struct ast base;

    enum ast_type type;

    struct ast *left;
    struct ast *right;
};

typedef void (*free_type)(struct ast *ast);
typedef void (*print_type)(struct ast *ast);

#endif /* ! AST_H */
