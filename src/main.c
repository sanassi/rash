#include "parser/parser.h"
#include "lexer/lexer.h"
#include "stream/stream.h"
#include "execution/execution.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    return 1;

    struct stream *stream = stream_open_string(argv[1]);
    struct lexer *l = lexer_init();
    l->stream = stream;
    struct parser *p = parser_init();
    p->lexer = l;

    struct ast *root = NULL;
    int status = parse_input(p, &root);

    if (status == PARSER_ERROR)
        printf("ERROR\n");

    print_ast(root);
    printf("\n");

    run_ast(root);

    free_ast(root);

    for (size_t i = 0; i < l->tokens->size; i++)
        token_free(l->tokens->nodes[i]->data);
    vector_free(l->tokens);
    lexer_free(l);
    stream_free(stream);
    parser_free(p);
    return 0;
}
