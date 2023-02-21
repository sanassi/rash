#include "repl.h"

struct stream *stream_open(struct program_args *args)
{
    if (args->file)
        return stream_open_file(args->file_path);
    if (args->string)
        return stream_open_string(args->str_input);
    if (args->input)
        return stream_open_stdin();
    errx(2, "invalid stream options");
}

int rash_repl(struct program_args *args, struct env *env)
{
    struct stream *stream = stream_open(args);
    struct lexer *l = lexer_init();
    l->stream = stream;

    int run_status = true_builtin();
    int parse_status = true_builtin();

    while (true)
    {
        struct parser *p = parser_init();
        p->lexer = l;
        struct ast *root = NULL;

        parse_status = parse_input(p, &root);

        if (parse_status == PARSER_ERROR && !args->input)
            errx(parse_status, "grammar error");

        if (args->pretty)
            print_ast(root);

        if (root)
            run_status = run_ast(root, env);

        if (run_status == BUILTIN_ERR && !args->input)
            errx(run_status, "execution error");

        fflush(stream->fp);

        bool done = p->is_at_end || run_status == EXIT_CODE;

        free_ast(root);
        parser_free(p);

        if (done)
            break;
    }

    for (size_t i = 0; i < l->tokens->size; i++)
        token_free(l->tokens->nodes[i]->data);

    int env_exit_value = run_status
        == EXIT_CODE ? env->exit_value : env->last_cmd_value;

    vector_free(l->tokens);
    lexer_free(l);
    stream_free(stream);

    return parse_status == PARSER_ERROR ? parse_status : env_exit_value;
}
