#ifndef REDIR_H
#define REDIR_H

#include "execution.h"
#define REDIR_EXIT_ERR 2

void redirect(struct vector *redirections); 

void undo_redirection(struct vector *redir_vect);

void redirection_restore_fd(struct ast *node, int stdout_dup,
                                   int stdin_dup, int stderr_dup);
#endif /* ! REDIR_H */
