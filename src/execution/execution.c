#include "execution.h"
#include "builtins/bool.h"
#include "redir.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pipeline.h"

static const char *builtins_str[] = {
    "true", "false", "echo", NULL
};

bool is_builtin(char *str)
{
    for (size_t i = 0 ; builtins_str[i]; i++)
    {
        if (!strcmp(builtins_str[i], str))
            return true;
    }
    return false;
}

struct builtin
{
    char *name;
    builtin_run exec_func;
};

int builin_execute(char *builtin_name, struct vector *args)
{
    if (str_equ(builtin_name, "true") || str_equ(builtin_name, "false"))
        return strcmp(builtin_name, "true") == 0 ? true_builtin() : false_builtin();

    static struct builtin builtins[] =
    {
        {"echo", &echo}
    };

    size_t nb_builtins = sizeof(builtins) / sizeof(struct builtin);
    int status = true_builtin();

    for (size_t i = 0; i < nb_builtins; i++)
    {
        if (!strcmp(builtins[i].name, builtin_name))
        {
            status = builtins[i].exec_func(args);
            break;
        }
    }

    fflush(stdout);
    return status;
}

int fork_and_execute(char **args)
{
    int status = true_builtin();
    int pid = fork();

    if (pid == 0)
    {
        execvp(args[0], args);
        err(BUILTIN_ERR, "invalid command %s", args[0]);
    }
    else
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
    }
    return true_builtin();
}

struct vector *expand_all(struct vector *input, struct env *env)
{
    struct vector *res = vector_new();
    for (size_t i = 0; i < input->size; i++)
    {
        struct vector *expanded = 
            expand(vector_get_at(input, i), env);
        for (size_t j = 0; j < expanded->size; j++)
        {
            char *str = vector_get_at(expanded, j);
            vector_append(&res, str, strlen(str) + 1);
        }
        vector_free(expanded);
    }
    return res;
}

int simple_cmd_execute(struct ast *ast, struct env *env)
{
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *) ast;
    struct vector *expanded_args = expand_all(simple_cmd->args, env);
    int status = true_builtin();

    if (simple_cmd->assignments)
    {
        for (size_t i = 0; i < simple_cmd->assignments->size; i++)
        {
            run_ast(vector_get_at(simple_cmd->assignments, 
                        i), env);
        }
    }

    int stdout_dup = dup(STDOUT_FILENO);
    int stdin_dup = dup(STDIN_FILENO);
    int stderr_dup = dup(STDERR_FILENO);

    redirect(simple_cmd->redir_pref);
    redirect(simple_cmd->redir_suff);

    if (expanded_args->size != 0)
    {
        char *cmd_name = vector_get_at(expanded_args, 0);

        if (is_builtin(cmd_name))
            status = builin_execute(cmd_name, expanded_args);
        else
        {
            char **args = vector_convert_str_arr(expanded_args, true);
            status = fork_and_execute(args);

            for (size_t i = 0; args[i]; i++)
                free(args[i]);
            free(args);
        }
    }

    redirection_restore_fd(ast, stdout_dup, stdin_dup, stderr_dup);

    for (size_t i = 0; i < expanded_args->size; i++)
        free(vector_get_at(expanded_args, i));
    vector_free(expanded_args);

    return status;
}

int cmd_list_execute(struct ast *ast, struct env *env)
{
    struct ast_cmd_list *list = (struct ast_cmd_list *) ast;
    int status = true_builtin();
    for (size_t i = 0; i < list->commands->size; i++)
        status = run_ast(vector_get_at(list->commands, i), env);

    return status;
}

int if_execute(struct ast *ast, struct env *env)
{
    struct ast_if *if_node = (struct ast_if *) ast;

    int status = true_builtin();
    int condition_status;

    if ((condition_status = run_ast(if_node->condition, env)) == true_builtin())
        status = run_ast(if_node->body, env);
    else if (if_node->else_body)
        status = run_ast(if_node->else_body, env);

    return status;
}

int cmd_execute(struct ast *ast, struct env *env)
{
    struct ast_cmd *cmd = (struct ast_cmd *) ast;
    
    int stdout_dup = dup(STDOUT_FILENO);
    int stdin_dup = dup(STDIN_FILENO);
    int stderr_dup = dup(STDERR_FILENO);

    redirect(cmd->redirections);

    int status = run_ast(cmd->command, env);

    undo_redirection(cmd->redirections);

    dup2(stdin_dup, STDIN_FILENO);
    dup2(stdout_dup, STDOUT_FILENO);
    dup2(stderr_dup, STDERR_FILENO);

    close(stdin_dup);
    close(stdout_dup);
    close(stderr_dup);

    return status;
}

int neg_execute(struct ast *ast, struct env *env)
{
    struct ast_neg *neg = (struct ast_neg *) ast;
    return run_ast(neg->pipeline, env) == 
        true_builtin() ? false_builtin() : true_builtin();
}

int and_or_execute(struct ast *ast, struct env *env)
{
    struct ast_and_or *and_or = (struct ast_and_or *) ast;
    int left_eval = run_ast(and_or->left, env);

    if (and_or -> type == AST_AND)
    {
        if (left_eval != true_builtin())
            return left_eval;
        return run_ast(and_or->right, env);
    }
    
    if (left_eval == true_builtin())
        return true_builtin();
    return run_ast(and_or->right, env);
}

int assignment_execute(struct ast *ast, struct env *env)
{
    struct ast_assign *assign = (struct ast_assign *) ast;
    struct vector *expanded_value = expand(assign->value, env);

    char *concat_value = NULL;
    for (size_t i = 0; i < expanded_value->size; i++)
    {
        char *value = vector_get_at(expanded_value, i);
        concat_value = my_str_cat(concat_value, value, strlen(value) + 1);
    }

    env_add_variable(env, assign->id, concat_value);

    free(concat_value);

    for (size_t i = 0; i < expanded_value->size; i++)
        free(vector_get_at(expanded_value, i));
    vector_free(expanded_value);

    return true_builtin();
}

int while_execute(struct ast *ast, struct env *env)
{
    struct ast_while *while_node = (struct ast_while *) ast;

    int status = true_builtin();
    while (run_ast(while_node->condition, env) == true_builtin())
    {
        status = run_ast(while_node->body, env);
    }

    return status;
}

int until_execute(struct ast *ast, struct env *env)
{
    struct ast_until *until_node = (struct ast_until *) ast;

    int status = true_builtin();
    while (run_ast(until_node->condition, env) == false_builtin())
    {
        status = run_ast(until_node->body, env);
    }

    return status;
}

int for_execute(struct ast *ast, struct env *env)
{
    struct ast_for *for_node = (struct ast_for *) ast;
    int status = true_builtin();

    struct vector *expanded_words = NULL;
    struct vector *args = for_node->words;

    if (!args)
    {
        for (size_t i = 0; i < env->argc; i++)
        {
            char *index_to_str  = my_itoa(i);
            char *value = env_get_variable(env, index_to_str);
            vector_append(&expanded_words, value, strlen(value) + 1);
            free(index_to_str);
        }
    }
    else
        expanded_words = expand_all(args, env);
    
    for (size_t i = 0; i < expanded_words->size; i++)
    {
        env_add_variable(env, for_node->loop_word, 
                vector_get_at(expanded_words, i));
        
        status = run_ast(for_node->body, env);
    }

    for (size_t i = 0; i < expanded_words->size; i++)
        free(vector_get_at(expanded_words, i));

    vector_free(expanded_words);

    return status;
}

int run_ast(struct ast *ast, struct env *env)
{
    if (!ast)
        return true_builtin();

    static const run_type run_functions[] = {
        [AST_SIMPLE_CMD] = &simple_cmd_execute,
        [AST_CMD_LIST] = &cmd_list_execute,
        [AST_IF] = &if_execute,
        [AST_CMD] = &cmd_execute,
        [AST_PIPE] = &pipe_execute,
        [AST_PIPELINE] = &pipeline_execute,
        [AST_NEG] = &neg_execute,
        [AST_AND_OR] = &and_or_execute,
        [AST_ASSIGN] = &assignment_execute,
        [AST_WHILE] = &while_execute,
        [AST_UNTIL] = &until_execute,
        [AST_FOR] = &for_execute
    };

    int status = (*run_functions[ast->type])(ast, env);

    char *status_to_str = my_itoa(status);
    env_add_variable(env, "?", status_to_str);
    free(status_to_str);

    return status;
}
