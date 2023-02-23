#include "redir.h"

static int get_flags(enum redirection_type redir_type)
{
    if (redir_type == REDIR_DGREAT)
        return O_CREAT | O_APPEND | O_WRONLY;

    if (redir_type == REDIR_LESS)
        return O_RDONLY;

    if (redir_type == REDIR_LESSAND)
        return O_CREAT | O_RDWR;

    return O_CREAT | O_TRUNC | O_RDWR;
}

/*
 * TODO : combine redirect functions
 * and clean up
 */

/*
 * Given the redirection node (which contains the redirection type),]
 * set the file descriptors, and open the file (Read and/or Write)
 */

static void set_file_descriptors(struct ast_redir *redir, int *old, int *fd)
{
    if (redir->redir_type == REDIR_LESSAND || redir->redir_type == REDIR_LESS)
    {
        *fd = STDIN_FILENO;
        if (redir->has_io_number)
            *fd = redir->io_number;
        if (redir->redir_type == REDIR_LESSAND)
        {
            if (str_is_number(redir->file))
                *old = atoi(redir->file);
            else
                errx(REDIR_EXIT_ERR, "redirect : invalid fd : WORD");
        }
        else
            *old = open(redir->file, get_flags(redir->redir_type), 00700);
    }

    if (redir->redir_type == REDIR_GREAT || redir->redir_type == REDIR_DGREAT
        || redir->redir_type == REDIR_GREATAND
        || redir->redir_type == REDIR_CLOBBER
        || redir->redir_type == REDIR_LESSGREAT)
    {
        *fd = STDOUT_FILENO;
        if (redir->has_io_number)
            *fd = redir->io_number;
        if (redir->redir_type == REDIR_LESSGREAT && !redir->has_io_number)
        {
            *old = open(redir->file, O_RDONLY | O_CREAT, 00700);
            *fd = STDIN_FILENO;
        }
        else if (redir->redir_type == REDIR_GREATAND)
        {
            if (str_is_number(redir->file))
                *old = atoi(redir->file);
            else
                errx(REDIR_EXIT_ERR, "redirect : invalid fd WORD");
        }
        else
            *old = open(redir->file, get_flags(redir->redir_type), 00700);
    }
}

void redirect(struct vector *redirections)
{
    if (!redirections)
        return;

    // struct ast_redir **redirs = (struct ast_redir **)nodes;

    for (size_t i = 0; i < redirections->size; i++)
    {
        int fd;
        int old;

        struct ast_redir *redir = vector_get_at(redirections, i);

        set_file_descriptors(redir, &old, &fd);
        redir->start_fd = dup(old);
        if (redir->start_fd == -1)
        {
            close(old);
            errx(false_builtin(), "Error redirect pref : dup");
        }

        fcntl(fd, F_SETFD, FD_CLOEXEC);

        int new_fd = dup2(old, fd);
        if (new_fd == -1)
        {
            close(fd);
            close(old);
            errx(false_builtin(), "Error redirect pref dup2 failed");
        }

        redir->new_fd = dup(fd);
    }
}

// not sure if necessary
void undo_redirection(struct vector *redir_vect)
{
    if (!redir_vect)
        return;

    size_t nb_nodes = redir_vect->size;

    for (size_t i = 0; i < nb_nodes; i++)
    {
        struct ast_redir *redir_node =
            (struct ast_redir *)vector_get_at(redir_vect, i);
        fflush(stdout);
        fflush(stderr);
        dup2(redir_node->start_fd, redir_node->new_fd);
        close(redir_node->new_fd);
        close(redir_node->start_fd);
    }
}

void redirection_restore_fd(struct ast *node, int stdout_dup, int stdin_dup,
                            int stderr_dup)
{
    struct ast_simple_cmd *sc_node = (struct ast_simple_cmd *)node;

    fflush(stdout);
    fflush(stderr);
    dup2(stdin_dup, STDIN_FILENO);
    dup2(stdout_dup, STDOUT_FILENO);
    dup2(stderr_dup, STDERR_FILENO);

    close(stdin_dup);
    close(stdout_dup);
    close(stderr_dup);
    undo_redirection(sc_node->redir_pref);
    undo_redirection(sc_node->redir_suff);
}
