#include "execution.h"
#include "builtins/bool.h"
#include "builtins/echo.h"
#include <stdio.h>
#include <string.h>

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
        return strcmp(builtin_name, "true");

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

int simple_cmd_execute(struct ast *ast)
{
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *) ast;

    char *cmd_name = vector_get_at(simple_cmd->args, 0);
    if (is_builtin(cmd_name))
    {
        return builin_execute(cmd_name, simple_cmd->args);
    }

    char **args = vector_convert_str_arr(simple_cmd->args, true);

    int status = fork_and_execute(args);

    for (size_t i = 0; args[i]; i++)
        free(args[i]);
    free(args);

    return status;
}

int cmd_list_execute(struct ast *ast)
{
    struct ast_cmd_list *list = (struct ast_cmd_list *) ast;
    int status = true_builtin();
    for (size_t i = 0; i < list->commands->size; i++)
        status = run_ast(vector_get_at(list->commands, i));

    return status;
}

int if_execute(struct ast *ast)
{
    struct ast_if *if_node = (struct ast_if *) ast;

    int status = true_builtin();
    int condition_status;

    if ((condition_status = run_ast(if_node->condition)) == true_builtin())
        status = run_ast(if_node->body);
    else if (if_node->else_body)
        status = run_ast(if_node->else_body);

    return status;
}

int run_ast(struct ast *ast)
{
    if (!ast)
        return true_builtin();

    static const run_type run_functions[] = {
        [AST_SIMPLE_CMD] = &simple_cmd_execute,
        [AST_CMD_LIST] = &cmd_list_execute,
        [AST_IF] = &if_execute,
    };

    return (*run_functions[ast->type])(ast);
}
