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
        printf("expected next token:\n");
        token_print(lexer_look_next_token(l));
        printf("got:\n");
        struct token *t = lexer_get_next_token(l);
        token_print(t);
        printf("\n");
    }

    stream_free(l->stream);
    for (size_t i = 0; i < l->tokens->size; i++)
        token_free(l->tokens->nodes[i]->data);
    vector_free(l->tokens);
    lexer_free(l);
    return 0;
}
