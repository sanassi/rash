#include "pipeline.h"

int pipe_exec_fork(struct ast *node, int fds[2],
                          enum pipe_side side)
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

    exit(run_ast(node));
}

int pipe_execute(struct ast *node)
{
    struct ast_pipe *pipe_node = (struct ast_pipe *)node;

    if (!pipe_node->left)
        return run_ast(pipe_node->right);
    if (!pipe_node->right)
        return run_ast(pipe_node->left);

    int fds[2];
    pipe(fds);

    int pid_left = pipe_exec_fork(pipe_node->left, fds, LEFT);
    int pid_right = pipe_exec_fork(pipe_node->right, fds, RIGHT);

    close(fds[0]);
    close(fds[1]);

    int status;

    waitpid(pid_left, &status, 0);
    waitpid(pid_right, &status, 0);

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return true_builtin();
}

int pipeline_execute(struct ast *node)
{
    struct ast_pipeline *pipeline_node = (struct ast_pipeline *)node;
    return run_ast(pipeline_node->pipe);
}
