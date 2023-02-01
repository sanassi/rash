#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    struct lexer *l = lexer_init();
    l->stream = stream_open_string(argv[1]);

    while (!l->is_at_end)
    {
        struct token *t = lexer_get_next_token(l);
        printf("%s\n", t->lexeme);
        token_free(t);
        /*
        char *res = lexer_advance(l);
        if (!res)
            continue;
        printf("%s\n", res);
        free(res);
        */
    }
    /*
    l->cur_token = calloc(1, sizeof(char *));
    *l->cur_token = strdup("<");

    if (char_can_form_operator(l, '&'))
        printf("OP\n");
    else
        printf("NOPE\n");
    */

    stream_free(l->stream);
    vector_free(l->tokens);
    lexer_free(l);
    return 0;
}
