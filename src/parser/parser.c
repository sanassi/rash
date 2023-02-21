#include "parser.h"
#include "ast.h"
#include "ast_free.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct parser *parser_init(void)
{
    struct parser *p = calloc(1, sizeof(struct parser));
    return p;
}

void parser_free(struct parser *p)
{
    free(p);
}

enum token_type parser_peek(struct parser *p)
{
    return lexer_look_next_token(p->lexer)->type;
}

static struct token *parser_advance(struct parser *p)
{
    if (!p)
        return NULL;

    if (p->is_at_end)
        return vector_get_at(p->lexer->tokens, p->lexer->tokens->size - 1);

    struct token *res = lexer_get_next_token(p->lexer);

    if (res -> type == END)
        p->is_at_end = true;

    return res;
}

static struct token *parser_previous(struct parser *p)
{
    return vector_get_at(p->lexer->tokens, p->lexer->current_index - 1);
}

static bool parser_check(struct parser *p, enum token_type expected_type)
{
    if (!p)
        errx(1, "Error - parser_check : parser is NULL");

    return parser_peek(p) == expected_type;
}

static bool parser_check_mult(struct parser *p, size_t count, ...)
{
    va_list ptr;
    va_start(ptr, count);

    for (size_t i = 0; i < count; i++)
    {
        if (parser_check(p, va_arg(ptr, int)))
        {
            va_end(ptr);
            return true;
        }
    }

    va_end(ptr);

    return false;
}

static bool parser_match(struct parser *p, size_t count, ...)
{
    va_list ptr;
    va_start(ptr, count);

    for (size_t i = 0; i < count; i++)
    {
        if (parser_check(p, va_arg(ptr, int)))
        {
            parser_advance(p);
            va_end(ptr);
            return true;
        }
    }

    va_end(ptr);

    return false;
}


#define AST_ALLOC(ast_type, enum_type)                                         \
    static struct ast_##ast_type *ast_##ast_type##_alloc(void)       \
    {                                                                          \
        struct ast_##ast_type *node =                                     \
            calloc(1, sizeof(struct ast_##ast_type));                     \
        (node->base).type = enum_type;                                         \
        return node;                                                           \
    }

AST_ALLOC(simple_cmd, AST_SIMPLE_CMD)
AST_ALLOC(cmd_list, AST_CMD_LIST)
AST_ALLOC(if, AST_IF)
AST_ALLOC(redir, AST_REDIR)
AST_ALLOC(cmd, AST_CMD)
AST_ALLOC(pipe, AST_PIPE)
AST_ALLOC(pipeline, AST_PIPELINE)
AST_ALLOC(neg, AST_NEG)
AST_ALLOC(and_or, AST_AND_OR)
AST_ALLOC(assign, AST_ASSIGN)
AST_ALLOC(while, AST_WHILE)
AST_ALLOC(until, AST_UNTIL)
AST_ALLOC(for, AST_FOR)

/*
 * forward declarations
 */

int parse_compound_list(struct parser *p, struct ast **res);
int parse_and_or(struct parser *p, struct ast **res);
int parse_if(struct parser *p, struct ast **res, bool expect_fi);
int parse_redirection(struct parser *p, struct ast **res);

int parse_for(struct parser *p, struct ast **res)
{
    struct ast_for *for_node = ast_for_alloc();
    *res = &(for_node->base);

    int status = PARSER_OK;

    if (!parser_match(p, 1, FOR))
        goto error;

    if  (!parser_match(p, 1, WORD))
        goto error;

    for_node->loop_word = strdup(parser_previous(p)->lexeme);
    
    if (parser_check(p, SCOLON))
        parser_advance(p);
    else
    {
        while (parser_match(p, 1, NEWLINE))
            continue;

        if (parser_match(p, 1, IN))
        {
            while (parser_match(p, 1, WORD))
            {
                char *word = strdup(parser_previous(p)->lexeme);
                vector_append(&(for_node->words), word, strlen(word) + 1);
            }

            parser_match(p, 2, NEWLINE, SCOLON);
        }
    }

    while (parser_match(p, 1, NEWLINE))
        continue;

    if (!parser_match(p, 1, DO))
        goto error;

    if ((status = parse_compound_list(p, &for_node->body)) != PARSER_OK)
        goto error;

    if (!parser_match(p, 1, DONE))
        goto error;

    return status;

error:
    return PARSER_ERROR;
}

int parse_while(struct parser *p, struct ast **res)
{
    struct ast_while *while_node = ast_while_alloc();
    *res = &(while_node->base);

    int status = PARSER_OK;

    if (!parser_match(p, 1, WHILE))
        goto error;

    if ((status = parse_compound_list(p, &(while_node->condition))) != PARSER_OK)
        goto error;

    if (!parser_match(p, 1, DO))
        goto error;

    if ((status = parse_compound_list(p, &(while_node->body))) != PARSER_OK)
        goto error;

    if (!parser_match(p, 1, DONE))
        goto error;

    return status;

error:
    fprintf(stderr, "error: parse while\n");
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_until(struct parser *p, struct ast **res)
{
    struct ast_until *until_node = ast_until_alloc();
    *res = &(until_node->base);

    int status = PARSER_OK;

    if (!parser_match(p, 1, UNTIL))
        goto error;

    if ((status = parse_compound_list(p, &(until_node->condition))) != PARSER_OK)
        goto error;

    if (!parser_match(p, 1, DO))
        goto error;

    if ((status = parse_compound_list(p, &(until_node->body))) != PARSER_OK)
        goto error;

    if (!parser_match(p, 1, DONE))
        goto error;

    return status;

error:
    fprintf(stderr, "error: parse until\n");
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_else(struct parser *p, struct ast **res)
{
    int status = PARSER_OK;

    if (parser_check(p, ELSE))
    {
        parser_advance(p);
        status = parse_compound_list(p, res);
    }
    else if (parser_check(p, ELIF)) 
    {
        parser_advance(p);
        status = parse_if(p, res, false);
    }
    else 
        return PARSER_ERROR;

    return status;
}

int parse_if(struct parser *p, struct ast **res, bool expect_fi)
{
    struct ast_if *if_node = ast_if_alloc();
    *res = &(if_node->base);

    int status = PARSER_OK;

    if (expect_fi && !parser_match(p, 1, IF))
        goto error;

    if ((status = parse_compound_list(p, &if_node->condition)) != PARSER_OK)
        goto error;

    if (!parser_match(p, 1, THEN))
        goto error;

    if ((status = parse_compound_list(p, &if_node->body)) != PARSER_OK)
        goto error;

    if (parser_check_mult(p, 2, ELSE, ELIF))
        status = parse_else(p, &if_node->else_body);

    if (expect_fi && !parser_match(p, 1, FI))
        goto error;

    return status;

error:
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_shell_command(struct parser *p, struct ast **res)
{
    int status = PARSER_OK;

    switch (parser_peek(p)) 
    {
    case IF:
        status = parse_if(p, res, true);
        break;
    case WHILE:
        status = parse_while(p, res);
        break;
    case UNTIL:
        status = parse_until(p, res);
        break;
    case FOR:
        status = parse_for(p, res);
        break;
    case LBRACE:
        parser_advance(p);
        status = parse_compound_list(p, res);
        if (status != PARSER_OK)
            goto error;
        if (!parser_match(p, 1, RBRACE))
            status = PARSER_ERROR;
        break;
    default:
        status = PARSER_ERROR;
        break;
    }

    if (status != PARSER_OK)
        goto error;

    return status;

error:
    fprintf(stderr, "error: parse shell command");
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_element(struct parser *p, struct ast **res)
{
    if (parser_match(p, 1, WORD))
        return PARSER_OK;
    else
        return parse_redirection(p, res);
}

int parse_redirection(struct parser *p, struct ast **res)
{
    struct ast_redir *redir = ast_redir_alloc();
    *res = &(redir->base);

    if (parser_match(p, 1, IONUMBER))
    {
        redir->io_number = atoi(parser_previous(p)->lexeme);
        redir->has_io_number = true;
    }

    if (!parser_match(p, 7, GREAT, LESS, DGREAT, GREATAND, 
                LESSAND, CLOBBER, LESSGREAT))
        goto error;

    // switch needs to be replaced (w/ macro, or just by including lexer.h)
    switch (parser_previous(p)->type) 
    {
    case LESS:
        redir->redir_type = REDIR_LESS;
        break;
    case GREAT:
        redir->redir_type = REDIR_GREAT;
        break;
    case LESSGREAT:
        redir->redir_type = REDIR_LESSGREAT;
        break;
    case DGREAT:
        redir->redir_type = REDIR_DGREAT;
        break;
    case GREATAND:
        redir->redir_type = REDIR_GREATAND;
        break;
    case LESSAND:
        redir->redir_type = REDIR_LESSAND;
        break;
    case CLOBBER:
        redir->redir_type  = REDIR_CLOBBER;
        break;
    default:
        break;
    }

    redir->redir_op = strdup(parser_previous(p)->lexeme);

    if (!parser_match(p, 1, WORD))
        goto error;

    redir->file = strdup(parser_previous(p)->lexeme);

    return PARSER_OK;

error:
    fprintf(stderr, "error : parse_redirection\n");
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_prefix(struct parser *p, struct ast **res)
{
    if (parser_match(p, 1, ASSIGNMENT_WORD))
    {
        struct ast_assign *assign = ast_assign_alloc();
        *res = &(assign->base);
        char *lexeme = parser_previous(p)->lexeme;

        char *equal_pos = strstr(lexeme, "=");
        size_t equal_index = equal_pos - lexeme;

        assign->id = get_substr(lexeme, 0, equal_index); 
        assign->value = strdup(equal_pos + 1); 
    }
    else if (parse_redirection(p, res) != PARSER_OK)
        goto error;

    return PARSER_OK;

error:
    fprintf(stderr, "error : parse_prefix\n");
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_simple_command(struct parser *p, struct ast **res)
{
    struct ast_simple_cmd *simple_cmd = ast_simple_cmd_alloc();
    simple_cmd->args = vector_new();
    *res = &(simple_cmd->base);
    struct ast *tmp = NULL;

    bool has_prefix = false;

    while (parser_check_mult(p, 9, ASSIGNMENT_WORD, IONUMBER, GREAT, 
                LESS, DGREAT, GREATAND, 
                LESSAND, CLOBBER, LESSGREAT))
    {
        has_prefix = true;
        if (parser_check(p, ASSIGNMENT_WORD))
        {
            if (parse_prefix(p, &tmp) != PARSER_OK)
                goto error;
            vector_append(&simple_cmd->assignments, tmp, sizeof(struct ast *));
        }
        else
        {
            if (parse_prefix(p, &tmp) != PARSER_OK)
                goto error;
            vector_append(&simple_cmd->redir_pref, tmp, sizeof(struct ast *));
        }
    }

    if (has_prefix && !parser_check(p, WORD))
        return PARSER_OK;
    /*
    if (!parser_match(p, 1, WORD))
        return PARSER_OK;
    */

    if (!parser_match(p, 1, WORD))
        goto error;

    char *cmd_name = parser_previous(p) -> lexeme;
    vector_append(&simple_cmd->args, strdup(cmd_name), strlen(cmd_name));

    while (parser_check_mult(p, 9, IONUMBER, WORD, GREAT, LESS, DGREAT, GREATAND, 
                LESSAND, CLOBBER, LESSGREAT))
    {
        if (parser_check(p, WORD))
        {
            parse_element(p, res);
            struct token *prev = parser_previous(p);
            char *arg = strdup(prev->lexeme);
            vector_append(&simple_cmd->args, arg, strlen(arg));
        }
        else
        {
            if (parse_element(p, &tmp) != PARSER_OK)
                goto error;
            vector_append(&simple_cmd->redir_suff, tmp, sizeof(struct ast *));
        }
    }

    return PARSER_OK;

error:
    fprintf(stderr, "error : parse_simple_command\n");
    free_ast(*res);
    *res = NULL;
    return PARSER_ERROR;
}

int parse_command(struct parser *p, struct ast **res)
{
    struct ast_cmd *ast_cmd = ast_cmd_alloc();
    ast_cmd->redirections = vector_new();
    *res = &(ast_cmd->base);

    int status = PARSER_OK;

    if (parser_check_mult(p, 5, IF, WHILE, UNTIL, FOR, LBRACE))
    {
        status = parse_shell_command(p, &(ast_cmd->command));

        while (parser_check_mult(p, 8, IONUMBER, GREAT, LESS, DGREAT, GREATAND, 
                    LESSAND, CLOBBER, LESSGREAT))
        {
            struct ast *tmp;
            status = parse_redirection(p, &tmp);
            if (status != PARSER_OK)
                break;
            vector_append(&(ast_cmd->redirections), tmp, sizeof(struct ast *));
        }
    }
    else
    {
        status = parse_simple_command(p, &(ast_cmd->command));
    }

    if (status != PARSER_OK)
        goto error;

    return status;

error:
    fprintf(stderr, "error: parse_command\n");
    free_ast(*res);
    *res = NULL;
    return status;
}

int parse_pipe(struct parser *p, struct ast **res)
{
    struct ast_pipe *pipe = ast_pipe_alloc();
    *res = &(pipe->base);

    int status = parse_command(p, &(pipe->left));

    while (parser_match(p, 1, PIPE))
    {
        while (parser_match(p, 1, NEWLINE))
            continue;

        status = parse_pipe(p, &(pipe->right));
        if (status != PARSER_OK)
            break;
    }

    if (status != PARSER_OK)
        goto error;

    return status;

error:
    fprintf(stderr, "error: parse_pipe\n");
    free_ast(*res);
    *res = NULL;
    return status;
}

int parse_pipeline(struct parser *p, struct ast **res)
{
    struct ast_pipeline *pipeline;
    int status = PARSER_OK;

    if (parser_match(p, 1, BANG))
    {
        struct ast_neg *neg = ast_neg_alloc();
        *res = &(neg->base);
        status = parse_pipe(p, &(neg->pipeline));
    }
    else
    {
        pipeline = ast_pipeline_alloc();
        *res = &(pipeline->base);
        status = parse_pipe(p, &(pipeline->pipe));
    }

    if (status != PARSER_OK)
        goto error;

    return status;
error:
    fprintf(stderr, "error: parse_pipeline\n");
    free_ast(*res);
    *res = NULL;
    return status;
}

int parse_and_or(struct parser *p, struct ast **res)
{
    struct ast *tmp = NULL;
    int status = PARSER_OK;

    if ((status = parse_pipeline(p, &tmp)) != PARSER_OK)
        goto error;

    if (!parser_check_mult(p, 2, AND_IF, OR_IF))
    {
        *res = tmp;
        return status;
    }

    struct ast_and_or *root = NULL;

    while (parser_check_mult(p, 2, AND_IF, OR_IF))
    {
        root = ast_and_or_alloc();
        struct token *t = parser_advance(p);
        root -> left = tmp;

        if (t->type == AND_IF)
            root -> type = AST_AND;
        else
            root->type = AST_OR;
        status = parse_pipeline(p, &root->right);
        if (status != PARSER_OK)
            break;
        tmp = (struct ast *) root;
    }

    *res = (struct ast *) tmp;

    return PARSER_OK;

error:
    fprintf(stderr, "error: parse and or\n");
    free_ast(*res);
    *res = NULL;
    return status;
}

int parse_compound_list(struct parser *p, struct ast **res)
{
    struct ast_cmd_list *compound_list = ast_cmd_list_alloc();
    *res = &(compound_list->base);
    compound_list->commands = vector_new();

    int status = PARSER_OK;
    struct ast *tmp = NULL;

    while (parser_match(p, 1, NEWLINE))
        continue;

    if ((status = parse_and_or(p, &tmp)) != PARSER_OK)
       goto error; 

    vector_append(&compound_list->commands, tmp, sizeof(struct ast *));

    while (parser_match(p, 2, SCOLON, NEWLINE))
    {
        while (parser_match(p, 1, NEWLINE))
            continue;

        if (parser_check_mult(p, 7, THEN, FI, ELIF, ELSE, DO, DONE, RBRACE))
            break;

        if ((status = parse_and_or(p, &tmp)) != PARSER_OK)
            goto error;

        vector_append(&compound_list->commands, tmp, sizeof(struct ast *));
    }

    return status;

error:
    fprintf(stderr, "parser error: parse_coumpound_list\n");
    free_ast(&compound_list->base);
    *res = NULL;
    return status;
}

int parse_list(struct parser *p, struct ast **res)
{
    struct ast_cmd_list *list = ast_cmd_list_alloc();
    *res = &(list->base);
    list->commands = vector_new();

    int status = PARSER_OK;

    struct ast *tmp = NULL;

    if ((status = parse_and_or(p, &tmp)) != PARSER_OK)
        goto error;

    vector_append(&list->commands, tmp, sizeof(struct ast *));

    while (parser_match(p, 1, SCOLON))
    {
        if (parser_check_mult(p, 2, END, NEWLINE))
            break;

        parse_and_or(p, &tmp);
        vector_append(&list->commands, tmp, sizeof(struct ast *));
    }

    return PARSER_OK;

error:
    fprintf(stderr, "parser error: parse_list\n");
    free_ast(*res);
    *res = NULL;
    return status;
}

int parse_input(struct parser *p, struct ast **res)
{
    if (parser_match(p, 2, NEWLINE, END))
        return PARSER_OK;

    int status = PARSER_OK;
    if ((status = parse_list(p, res)) != PARSER_OK)
        return status;
    if (!parser_match(p, 2, NEWLINE, END))
        return PARSER_ERROR;
    return status;
}
