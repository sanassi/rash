#include "lexer.h"
#include <stdio.h>

#define HASH_MAP_DEFAULT_SIZE 50


static const char *tokens_str[] =
{
    "<<", ">>", "<&", ">&", "<>", "<<-", ">|", "(",
    ")", "|", "&", ";", "\n", ";;", "&&", "||",
    "if", "then", "else", "elif", "fi", "do", "done",
    "case", "esac", "while", "until", "for", "{", "}",
    "!", "in", "<", ">", NULL,
};
static const char *operators[] =
{
    "&", "&&", "(", ")", ";", ";;", "\n", "|",
    "||", ">", "<", ">|", "<<", ">>", "<&",
    ">&", NULL,
};

static struct hash_map *get_operators(void)
{
    struct hash_map *operators = hash_map_init(HASH_MAP_DEFAULT_SIZE);
    bool updated = false;
    // control
    hm_insert_int(operators, "&", AND, &updated);
    hm_insert_int(operators, "&&", AND_IF, &updated);
    hm_insert_int(operators, "(", LPAR, &updated);
    hm_insert_int(operators, ")", RPAR, &updated);
    hm_insert_int(operators, ";;", DSEMI, &updated);
    hm_insert_int(operators, ";", SCOLON, &updated);
    hm_insert_int(operators, "\n", NEWLINE, &updated);
    hm_insert_int(operators, "|", PIPE, &updated);
    hm_insert_int(operators, "||", OR_IF, &updated);
    // redirections
    hm_insert_int(operators, "<", LESS, &updated);
    hm_insert_int(operators, ">", GREAT, &updated);
    hm_insert_int(operators, "<>", LESSGREAT, &updated);
    hm_insert_int(operators, ">|", CLOBBER, &updated);
    hm_insert_int(operators, "<<", DLESS, &updated);
    hm_insert_int(operators, ">>", DGREAT, &updated);
    hm_insert_int(operators, "<&", LESSAND, &updated);
    hm_insert_int(operators, ">&", GREATAND, &updated);

    return operators;
}

static struct hash_map *get_reserved_words(void)
{
    struct hash_map *reserved = hash_map_init(HASH_MAP_DEFAULT_SIZE);
    bool updated = false;

    hm_insert_int(reserved, "if", IF, &updated);
    hm_insert_int(reserved, "then", THEN, &updated);
    hm_insert_int(reserved, "else", ELSE, &updated);
    hm_insert_int(reserved, "elif", ELIF, &updated);
    hm_insert_int(reserved, "fi", FI, &updated);
    hm_insert_int(reserved, "do", DO, &updated);
    hm_insert_int(reserved, "done", DONE, &updated);
    hm_insert_int(reserved, "case", CASE, &updated);
    hm_insert_int(reserved, "esac", ESAC, &updated);
    hm_insert_int(reserved, "while", WHILE, &updated);
    hm_insert_int(reserved, "until", UNTIL, &updated);
    hm_insert_int(reserved, "for", FOR, &updated);
    hm_insert_int(reserved, "{", LBRACE, &updated);
    hm_insert_int(reserved, "}", RBRACE, &updated);
    hm_insert_int(reserved, "!", BANG, &updated);
    hm_insert_int(reserved, "in", IN, &updated);
    return reserved;
}

struct lexer *lexer_init(void)
{
    struct lexer *l = calloc(1, sizeof(struct lexer));

    l->operators = get_operators();
    l->reserved_words = get_reserved_words();
    l->cur_token = calloc(1, sizeof(char *));
    l->tokens = vector_new();

    return l;
}

void lexer_free(struct lexer *l)
{
    hash_map_free(l->operators);
    hash_map_free(l->reserved_words);

    free(*l->cur_token);
    free(l->cur_token);
    free(l->full_input);

    free(l);
}

bool char_can_form_operator(struct lexer *l, char c)
{
    if (!*l->cur_token)
    {
        for (size_t i = 0; operators[i]; i++)
        {
            if (fnmatch(&c, operators[i], 0) == 0)
                return true;
        }
        return false;
    }

    char *prev = strdup(*l->cur_token);
    char *potential_op = my_str_cat(prev, &c, 1);
    bool res = false;

    char *pattern = NULL;
    pattern = my_str_cat(pattern, potential_op, strlen(potential_op));

    for (size_t i = 0; operators[i]; i++)
    {
        if (fnmatch(pattern, operators[i], 0) == 0)
        {
            res = true;
            break;
        }
    }

    free(pattern);
    free(potential_op);
    return res;
}


static bool char_can_start_operator(char c)
{
    for (size_t i = 0; operators[i]; i++)
    {
        if (c == operators[i][0])
            return true;
    }

    return false;
}

static bool is_quoting_char(char c)
{
    return c == '\'' || c == '"' || c == '\\';
}

/**
 * Performs token recognition based on the algorithm described in
 * the SCL.
 *
 * Returns NULL if the current token is a blank character.
 **/

/*
 * TODO : Function needs to be refactored
 */

char *lexer_advance(struct lexer *l)
{
    char c = stream_advance(l->stream);
    char *res = NULL;

    if (c == EOF)
    {
        l->is_at_end = true;
        if (!*l->cur_token)
            return NULL;
        return strdup(*l->cur_token);
    }

    if (l->prev_char_in_op && !l->quoting && char_can_form_operator(l, c))
    {
        *l->cur_token = my_str_cat(*l->cur_token, &c, 1);
        l->prev_char_in_op = true;

        return lexer_advance(l);
    }

    if (l->prev_char_in_op && !char_can_form_operator(l, c))
    {
        if (*l->cur_token)
            res = strdup(*l->cur_token);
        free(*l->cur_token);
        *l->cur_token = NULL;
        l->prev_char_in_op = false;

        if (!isblank(c))
        {
            *l->cur_token = my_str_cat(*l->cur_token, &c, 1);
            // TODO : TEST THIS
            if (char_can_start_operator(c))
                l->prev_char_in_op = true;
        }
        return res;
    }

     //quoting

    if (!l->quoting && is_quoting_char(c))
    {
        l->quoting = true;
        char cur = c;
        *l->cur_token = my_str_cat(*l->cur_token, &cur, 1);
        if (c == '\\')
        {
            l->quoting = false;
            return lexer_advance(l);
        }

        while (stream_peek(l->stream) != c)
        {
            cur = stream_advance(l->stream);
            *l->cur_token = my_str_cat(*l->cur_token, &cur, 1);
            if (cur == '\\' && c == '"')
            {
                cur = stream_advance(l->stream);
                *l->cur_token = my_str_cat(*l->cur_token, &cur, 1);               
            }
        }
        cur = stream_advance(l->stream);
        *l->cur_token = my_str_cat(*l->cur_token, &cur, 1);
        l->quoting = false;
        return lexer_advance(l);
    }

    
     //expansion $..
     

    if (!l->quoting && char_can_start_operator(c))
    {
        // *l->cur_token = my_str_cat(*l->cur_token, &c, 1);
        if (*l->cur_token)
            res = strdup(*l->cur_token);
        free(*l->cur_token);
        *l->cur_token = NULL;

        l->prev_char_in_op = true;
        *l->cur_token = calloc(2, sizeof(char));
        *l->cur_token[0] = c;
        return res;
    }

    if (!l->quoting && isblank(c))
    {
        if (*l->cur_token)
            res = strdup(*l->cur_token);
        free(*l->cur_token);
        *l->cur_token = NULL;
        return res;
    }

    if (l->prev_char_in_word)
    {
        *l->cur_token = my_str_cat(*l->cur_token, &c, 1);
        return lexer_advance(l);
    }

    if (c == '#')
    {
        while (stream_peek(l->stream) != '\n')
            stream_advance(l->stream);
        return lexer_advance(l);
    }

    *l->cur_token = my_str_cat(*l->cur_token, &c, 1);
    l->prev_char_in_word = true;

    return lexer_advance(l);
}

bool check_io_number(struct lexer *l, char *str)
{
    if (!str)
        return false;

    if (!str_is_number(str))
        return false;

    if (*l->cur_token && (*l->cur_token[0] == '<' || *l->cur_token[0] == '>'))
        return true;

    return false;
}

bool check_assign(char *str)
{
    char *equal_sign_pos = strstr(str, "=");
    if (!equal_sign_pos)
        return false;

    if (equal_sign_pos == str)
        return false;

    /*
     * check if name is valid
     */

    return true;
}

struct token *lexer_create_token(struct lexer *l, char *str)
{
    if (!str)
        return NULL;

    struct token *t = calloc(1, sizeof(struct token));
    t->lexeme = strdup(str);

    int type = hm_get_int(l->operators, t->lexeme);

    // token is an operator
    if (type >= 0)
    {
        t->type = type;
        return t;
    }

    if (check_io_number(l, t->lexeme))
    {
        t->type = IONUMBER;
        return t;
    }

    if (check_assign(str) && !l->context_is_set)
    {
        t->type = ASSIGNMENT_WORD;
        return t;
    }

    type = hm_get_int(l->reserved_words, t->lexeme);
    if (type < 0 || l->context_is_set)
        type = WORD;
    
    t->type = type;

    return t;
}

/*
 * Returns the next token.
 * The token vector is used as a buffer, if we are at the end,
 * call lexer_advance and create the next token,
 * otherwise, get the next token (already created) from the token
 * vector (at current_index).
 * 
 * It can be useful when we just want to look at the next token,
 * without eating it.
 */

struct token *lexer_get_next_token(struct lexer *l)
{
    if (l->current_index < l->tokens->size)
    {
        struct token *t = vector_get_at(l->tokens, l->current_index);
        l->current_index += 1;
        return t;
    }

    char *lexeme = NULL;
    struct token *res = NULL;

    if (l->is_at_end)
    {
        //return vector_get_at(l->tokens, l->tokens->size - 1);
        
        res = calloc(1, sizeof(struct token));
        res->type = END;
        res->lexeme = strdup("$$$");
        vector_append(&l->tokens, res, sizeof(struct token));
        l->current_index += 1;
        return res;
    }

    while (!lexeme)
    {
        if (l->is_at_end)
            break;
        lexeme = lexer_advance(l);
    }

    if (!lexeme && l->is_at_end)
    {
        res = calloc(1, sizeof(struct token));
        res->type = END;
        res->lexeme = strdup("$$$");
    }
    else
    {
        res = lexer_create_token(l, lexeme);
        free(lexeme);

        /*
        if (l->is_at_end)
        {
            struct token *end = calloc(1, sizeof(struct token));
            end->type = END;
            end -> 
        }
        */
    }

    vector_append(&l->tokens, res, sizeof(struct token));
    l->current_index += 1;

    return res;
}

struct token *lexer_look_next_token(struct lexer *l)
{
    struct token *res = lexer_get_next_token(l);
    /*TODO : CHECK !!!!!!!!!!!*/
    /*------------------------*/
    if (l->current_index != 0)
        l->current_index -= 1;
    return res;
}

void token_free(struct token *t)
{
    free(t->lexeme);
    free(t->value);
    free(t);
}

void token_print(struct token *t)
{
    switch (t->type)
    {
        case WORD:
            printf("%s [WORD]\n", t->lexeme);
            break;
        case NEWLINE:
            printf("\\n [NEWLINE]\n");
            break;
        case ASSIGNMENT_WORD:
            printf("%s [ASSIGNMENT_WORD]\n", t->lexeme);
            break;
        case IONUMBER:
            printf("%s [IONUMBER]\n", t->lexeme);
            break;
        case END:
            printf("END\n");
            break;
        default:
            printf("%s [%s]\n", t->lexeme, tokens_str[t->type]);
    }
}
