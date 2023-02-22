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

    if (simple_cmd->assignments)
    {
        for (size_t i = 0; i < simple_cmd->assignments->size; i++)
            print_ast(vector_get_at(simple_cmd->assignments, i));
    }

    if (simple_cmd->args && simple_cmd->args->size != 0)
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

static void print_assign(struct ast *ast)
{
    struct ast_assign *assign = (struct ast_assign *) ast;
    printf("%s = %s", assign->id, assign->value);
}

static void print_while(struct ast *ast)
{
    struct ast_while *while_node = (struct ast_while *) ast;
    printf("while ");
    print_ast(while_node->condition);
    printf("do\n");
    print_ast(while_node->body);
    printf("done\n");
}

static void print_until(struct ast *ast)
{
    struct ast_until *until_node = (struct ast_until *) ast;
    printf("while ");
    print_ast(until_node->condition);
    printf("do\n");
    print_ast(until_node->body);
    printf("done\n");
}

static void print_for(struct ast *ast)
{
    struct ast_for *for_node = (struct ast_for *) ast;

    printf("for %s\n", for_node->loop_word);
    
    if (for_node->words)
    {
        printf("in\n");
        for (size_t i = 0; i < for_node->words->size; i++)
            printf("%s ", (char *) vector_get_at(for_node->words, i));
        printf("\n");
    }

    printf("do ");
    print_ast(for_node->body);
    printf("\ndone\n");
}

static void print_func(struct ast *ast)
{
    struct ast_func *func = (struct ast_func *) ast;

    printf("%s()\n{\n", func->name);
    print_ast(func->body);
    printf("\n}");
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
        [AST_AND_OR] = &print_and_or,
        [AST_ASSIGN] = &print_assign,
        [AST_UNTIL] = &print_until,
        [AST_WHILE] = &print_while,
        [AST_FOR] = &print_for,
        [AST_FUNC] = &print_func
    };

    (*print_functions[ast->type])(ast);
}
