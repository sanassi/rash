#include "ast_free.h"
#include "ast.h"
#include <stdlib.h>

static void free_simple_cmd(struct ast *ast)
{
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *) ast;

    for (size_t i = 0; i < simple_cmd->args->size; i++)
        free(vector_get_at(simple_cmd->args, i));

    vector_free(simple_cmd->args);
    free(simple_cmd);
}

static void free_cmd_list(struct ast *ast)
{
    struct ast_cmd_list *list = (struct ast_cmd_list *) ast;

    for (size_t i = 0; i < list->commands->size; i++)
    {
        free_ast(vector_get_at(list->commands, i));
    }

    vector_free(list->commands);
    free(list);
}

static void free_if(struct ast *ast)
{
    struct ast_if *if_node = (struct ast_if *) ast;

    free_ast(if_node->condition);
    free_ast(if_node->body);

    if (if_node->else_body)
        free_ast(if_node->else_body);
    free(if_node);
}

void free_ast(struct ast *ast)
{
    static const free_type free_functions[] = 
    {
        [AST_SIMPLE_CMD] = &free_simple_cmd,
        [AST_CMD_LIST] = &free_cmd_list,
        [AST_IF] = &free_if
    };

    (*free_functions[ast->type])(ast);
}
