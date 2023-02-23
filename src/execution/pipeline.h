#ifndef PIPELINE_H
#define PIPELINE_H

#include "execution.h"

enum pipe_side
{
    LEFT = 0,
    RIGHT = 1
};

int pipe_exec_fork(struct ast *node, int fds[2], enum pipe_side side,
                   struct env *env);

int pipe_execute(struct ast *node, struct env *env);

int pipeline_execute(struct ast *node, struct env *env);

#endif /* ! PIPELINE_H */
