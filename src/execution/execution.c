#include "execution.h"
#include "builtins/bool.h"

char **vector_convert_str_arr(struct vector *v, bool add_null)
{
    char **res = NULL;
    size_t i = 0;
    for (; i < v->size; i++)
    {
        char *copy = strdup(vector_get_at(v, i));
        char **tmp = realloc(res, (i + 1) * sizeof(char *));
        if (tmp)
            res = tmp;
        res[i] = copy;
    }

    if (add_null)
    {
        char **tmp = realloc(res, (i + 1) * sizeof(char *));
        if (tmp)
            res = tmp;
        res[i] = NULL;
    }

    return res;
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
    {
        status = run_ast(vector_get_at(list->commands, i));
    }

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
