#include "ast_free.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

static void free_simple_cmd(struct ast *ast)
{
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *) ast;

    if (simple_cmd->args)
    {
        for (size_t i = 0; i < simple_cmd->args->size; i++)
            free(vector_get_at(simple_cmd->args, i));
    }

    if (simple_cmd->redir_pref)
    {
        for (size_t i = 0; i < simple_cmd->redir_pref->size; i++)
            free_ast(vector_get_at(simple_cmd->redir_pref, i));
        vector_free(simple_cmd->redir_pref);
    }

    if (simple_cmd->redir_suff)
    {
        for (size_t i = 0; i < simple_cmd->redir_suff->size; i++)
            free_ast(vector_get_at(simple_cmd->redir_suff, i));
        vector_free(simple_cmd->redir_suff);
    }

    vector_free(simple_cmd->args);
    free(simple_cmd);
}

static void free_cmd_list(struct ast *ast)
{
    struct ast_cmd_list *list = (struct ast_cmd_list *) ast;

    if (list->commands)
    {
        for (size_t i = 0; i < list->commands->size; i++)
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

static void free_redir(struct ast *ast)
{
    struct ast_redir *redir = (struct ast_redir *) ast;
    free(redir->redir_op);
    free(redir->file);

    free(redir);
}

static void free_cmd(struct ast *ast)
{
    struct ast_cmd *cmd = (struct ast_cmd *) ast;
    free_ast(cmd->command);

    if (cmd->redirections)
    {
        for (size_t i = 0; i < cmd->redirections->size; i++)
            free_ast(vector_get_at(cmd->redirections, i));
    }

    vector_free(cmd->redirections);
    free(cmd);
}

static void free_pipe(struct ast *ast)
{
    struct ast_pipe *pipe = (struct ast_pipe *) ast;
    free_ast(pipe->left);
    free_ast(pipe->right);

    free(pipe);
}

static void free_pipeline(struct ast *ast)
{
    struct ast_pipeline *pipeline = (struct ast_pipeline *) ast;
    free_ast(pipeline->pipe);

    free(pipeline);
}

static void free_neg(struct ast *ast)
{
    struct ast_neg *neg = (struct ast_neg *) ast;
    free_ast(neg->pipeline);
    free(neg);
}

static void free_and_or(struct ast *ast)
{
    struct ast_and_or *and_or = (struct ast_and_or *) ast;

    free_ast(and_or->left);
    if (and_or->right)
        free_ast(and_or->right);

    free(and_or);
}

void free_ast(struct ast *ast)
{
    if (!ast)
        return;

    static const free_type free_functions[] = 
    {
        [AST_SIMPLE_CMD] = &free_simple_cmd,
        [AST_CMD_LIST] = &free_cmd_list,
        [AST_IF] = &free_if,
        [AST_REDIR] = &free_redir,
        [AST_CMD] = &free_cmd,
        [AST_PIPE] = &free_pipe,
        [AST_PIPELINE] = &free_pipeline,
        [AST_NEG] = &free_neg,
        [AST_AND_OR] = &free_and_or
    };

    (*free_functions[ast->type])(ast);
}
