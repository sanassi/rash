#ifndef LEXER_H
#define LEXER_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "../stream/stream.h"
#include <fnmatch.h>
#include <ctype.h>

enum token_type
{
    /*operators*/

    //redirection operators
    DLESS,
    DGREAT,
    LESSAND,
    GREATAND,
    LESSGREAT,
    DLESSDASH,
    CLOBBER,
    //control operators
    LPAR,
    RPAR,
    PIPE,
    AND,
    SCOLON,
    NEWLINE,
    DSEMI,
    AND_IF,
    OR_IF,

    /*reserved words*/
    IF,
    THEN,
    ELSE,
    ELIF,
    FI,
    DO,
    DONE,
    CASE,
    ESAC,
    WHILE,
    UNTIL,
    FOR,
    LBRACE,
    RBRACE,
    BANG,
    IN,

    /*misc*/
    LESS,
    GREAT,

    IONUMBER,
    NAME,
    ASSIGNMENT_WORD,
    WORD,

    /*quote types*/
    DQUOTE,
    SQUOTE,

    /*end of input*/
    END
};

struct token
{
    enum token_type type;
    char *lexeme;
    char *value;
};

struct lexer
{
    bool is_at_end;
    bool prev_char_in_op;
    bool quoting;
    bool prev_char_in_word;
    bool is_escaping;

    bool context_is_set;

    char **cur_token;
    char *full_input;

    struct hash_map *operators;
    struct hash_map *reserved_words;

    struct vector *tokens;
    struct stream *stream;
};

struct lexer *lexer_init(void);
bool char_can_form_operator(struct lexer *l, char c);
void lexer_free(struct lexer *l);
char *lexer_advance(struct lexer *l);
struct token *lexer_get_next_token(struct lexer *l);
void token_free(struct token *t); 
#endif /* ! LEXER_H */
