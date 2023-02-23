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

    if (simple_cmd->assignments)
    {
        for (size_t i = 0; i < simple_cmd->assignments->size; i++)
            free_ast(vector_get_at(simple_cmd->assignments, i));
        vector_free(simple_cmd->assignments);
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

static void free_assign(struct ast *ast)
{
    struct ast_assign *assign = (struct ast_assign *) ast;
    free(assign->id);
    free(assign->value);

    free(assign);
}

static void free_while(struct ast *ast)
{
    struct ast_while *while_node = (struct ast_while *) ast;
    free_ast(while_node->body);
    free_ast(while_node->condition);

    free(while_node);
}

static void free_until(struct ast *ast)
{
    struct ast_until *until_node = (struct ast_until *) ast;
    free_ast(until_node->body);
    free_ast(until_node->condition);

    free(until_node);
}

static void free_for(struct ast *ast)
{
    struct ast_for *for_node = (struct ast_for *) ast;
    free(for_node->loop_word);

    if (for_node->words)
    {
        for (size_t i = 0; i < for_node->words->size; i++)
            free(vector_get_at(for_node->words, i));
        vector_free(for_node->words);
    }

    free_ast(for_node->body);
    free(for_node);
}

static void free_func(struct ast *ast)
{
    struct ast_func *func = (struct ast_func *) ast;
    if (func->nb_references != 0)
        return;

    free(func->name);

    free_ast(func->body);

    free(func);
}

static void free_subshell(struct ast *ast)
{
    struct ast_subshell *subshell = (struct ast_subshell *) ast;
    free_ast(subshell->compound_list);

    free(subshell);
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
        [AST_AND_OR] = &free_and_or,
        [AST_ASSIGN] = &free_assign,
        [AST_WHILE] = &free_while,
        [AST_UNTIL] = &free_until,
        [AST_FOR] = &free_for,
        [AST_FUNC] = &free_func,
        [AST_SUBSHELL] = &free_subshell
    };

    (*free_functions[ast->type])(ast);
}
