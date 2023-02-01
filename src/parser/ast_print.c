#include "ast_print.h"
#include "ast.h"
#include <stdio.h>

static void print_simple_command(struct ast *ast)
{
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *) ast;

    printf(" %s ", (char *) vector_get_at(simple_cmd->args, 0));

    for (size_t i = 1; i < simple_cmd->args->size; i++)
        printf(" %s ", (char *) vector_get_at(simple_cmd->args, i));
}

static void print_cmd_list(struct ast *ast)
{
    struct ast_cmd_list *list = (struct ast_cmd_list *) ast;

    for (size_t i = 0; i < list->commands->size; i++)
    {
        print_ast(vector_get_at(list->commands, i));
        printf(" ; ");
    }
}

static void print_ast_if(struct ast *ast)
{
    struct ast_if *if_node = (struct ast_if *) ast;
    printf(" if { ");
    print_ast(if_node->condition);
    printf(" }; then { ");
    print_ast(if_node->body);
    printf(" }; ");
    if (if_node->else_body)
    {
        printf("else { ");
        print_ast(if_node->else_body);
        printf(" }; ");
    }
    printf("fi");
}

void print_ast(struct ast *ast)
{
    static const print_type print_functions[] =
    {
        [AST_SIMPLE_CMD] = &print_simple_command,
        [AST_CMD_LIST] = &print_cmd_list,
        [AST_IF] = &print_ast_if
    };

    (*print_functions[ast->type])(ast);
}
