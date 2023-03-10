#include "pipeline.h"

#include "builtins/bool.h"

int pipe_exec_fork(struct ast *node, int fds[2], enum pipe_side side,
                   struct env *env)
{
    if (!node)
        return true_builtin();

    fflush(stdout);
    int pid = fork();
    if (pid != 0)
        return pid;

    enum pipe_side other_side = LEFT;
    if (side == LEFT)
        other_side = RIGHT;

    if (close(fds[side]) == -1)
        errx(false_builtin(), "exec fork failure %i", fds[side]);

    dup2(fds[other_side], side == LEFT ? STDOUT_FILENO : STDIN_FILENO);

    close(fds[other_side]);

    exit(run_ast(node, env));
}

int pipe_execute(struct ast *node, struct env *env)
{
    struct ast_pipe *pipe_node = (struct ast_pipe *)node;

    if (!pipe_node->left)
        return run_ast(pipe_node->right, env);
    if (!pipe_node->right)
        return run_ast(pipe_node->left, env);

    int fds[2];
    pipe(fds);

    int pid_left = pipe_exec_fork(pipe_node->left, fds, LEFT, env);
    int pid_right = pipe_exec_fork(pipe_node->right, fds, RIGHT, env);

    close(fds[0]);
    close(fds[1]);

    int status = true_builtin();

    waitpid(pid_left, &status, 0);
    waitpid(pid_right, &status, 0);

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return true_builtin();
}

int pipeline_execute(struct ast *node, struct env *env)
{
    struct ast_pipeline *pipeline_node = (struct ast_pipeline *)node;
    return run_ast(pipeline_node->pipe, env);
}
