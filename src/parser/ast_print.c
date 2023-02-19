#include "ast_print.h"
#include "ast.h"
#include <stdio.h>

static void print_simple_command(struct ast *ast)
{
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *) ast;

    if (simple_cmd->redir_pref)
    {
        for (size_t i = 0; i < simple_cmd->redir_pref->size; i++)
            print_ast(vector_get_at(simple_cmd->redir_pref, i));
    }

    printf(" %s ", (char *) vector_get_at(simple_cmd->args, 0));

    for (size_t i = 1; i < simple_cmd->args->size; i++)
        printf(" %s ", (char *) vector_get_at(simple_cmd->args, i));

    if (simple_cmd->redir_suff)
    {
        for (size_t i = 0; i < simple_cmd->redir_suff->size; i++)
            print_ast(vector_get_at(simple_cmd->redir_suff, i));
    }
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

static void print_redir(struct ast *ast)
{
    struct ast_redir *redir = (struct ast_redir *) ast;
    
    if (redir -> has_io_number)
        printf("%i", redir->io_number);
    printf(" %s %s ", redir->redir_op, redir->file);
}

static void print_cmd(struct ast *ast)
{
    struct ast_cmd *cmd = (struct ast_cmd *) ast;

    print_ast(cmd->command);

    if (cmd->redirections)
    {
        for (size_t i = 0; i < cmd->redirections->size; i++)
            print_ast(vector_get_at(cmd->redirections, i));
    }
}

static void print_pipe(struct ast *ast)
{
    struct ast_pipe *pipe = (struct ast_pipe *) ast;
    print_ast(pipe->left);

    if (pipe->right)
    {
        printf(" | ");
        print_ast(pipe->right);
    }
}

static void print_pipeline(struct ast *ast)
{
    struct ast_pipeline *pipeline = (struct ast_pipeline *) ast;
    print_ast(pipeline->pipe);
}

static void print_neg(struct ast *ast)
{
    struct ast_neg *neg = (struct ast_neg *) ast;
    printf(" ! ");
    print_ast(neg->pipeline);
}

static void print_and_or(struct ast *ast)
{
    struct ast_and_or *and_or = (struct ast_and_or *) ast;
    print_ast(and_or->left);

    if (and_or->right)
    {
        printf("%s", and_or->type == AST_AND ? "&&" : "||");
        print_ast(and_or->right);
    }
}

void print_ast(struct ast *ast)
{
    if (!ast)
        return;
    static const print_type print_functions[] =
    {
        [AST_SIMPLE_CMD] = &print_simple_command,
        [AST_CMD_LIST] = &print_cmd_list,
        [AST_IF] = &print_ast_if,
        [AST_REDIR] = &print_redir,
        [AST_CMD] = &print_cmd,
        [AST_PIPE] = &print_pipe,
        [AST_PIPELINE] = &print_pipeline,
        [AST_NEG] = &print_neg,
        [AST_AND_OR] = &print_and_or
    };

    (*print_functions[ast->type])(ast);
}
