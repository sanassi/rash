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

struct parser
{
    struct lexer *lexer;
    bool is_at_end;
};
int parse_input(struct parser *p, struct ast **res);
struct parser *parser_init(void);
void parser_free(struct parser *p);
#endif /* ! PARSER_H */
