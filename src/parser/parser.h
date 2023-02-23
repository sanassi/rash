#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "ast_print.h"
#include "ast_free.h"
#include "../utils/utils.h"
#include "../lexer/lexer.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PARSER_ERROR 2
#define PARSER_OK 0
#define MAX_LEN_RULE

struct parser
{
    struct lexer *lexer;
    bool is_at_end;

    bool debug;
};

enum grammar_rule
{
    INPUT,
    LIST,
    AND_OR,
    PIPELINE,
    COMMAND,
    SIMPLE_COMMAND,
    SHELL_COMMAND,
    FUNCDEC,
    REDIRECTION,
    PREFIX,
    ELEMENT,
    COMPOUND_LIST,
    FOR_RULE,
    WHILE_RULE,
    UNTIL_RULE,
    CASE_RULE,
    IF_RULE,
    ELSE_CLAUSE,
    CASE_CLAUSE,
    CASE_ITEM
};


int parse_input(struct parser *p, struct ast **res);
struct parser *parser_init(void);
void parser_free(struct parser *p);
#endif /* ! PARSER_H */
