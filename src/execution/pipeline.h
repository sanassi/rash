#ifndef PIPELINE_H
#define PIPELINE_H

#include "execution.h"

enum pipe_side
{
    LEFT = 0,
    RIGHT = 1
};

int pipe_exec_fork(struct ast *node, int fds[2],
                          enum pipe_side side);

int pipe_execute(struct ast *node);

int pipeline_execute(struct ast *node);

#endif /* ! PIPELINE_H */
