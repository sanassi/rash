#include "stream.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

struct stream *stream_open_string(char *string)
{
    struct stream *s = calloc(1, sizeof(struct stream));
    s->type = input_type_string;

    FILE *fp = fmemopen(string, strlen(string), "r");
    if (!fp)
        errx(EXIT_FAILURE, "Error : stream_open_string - file pointer NULL");
    s->fp = fp;
    return s;
}

struct stream *stream_open_file(char *path)
{
    struct stream *s = calloc(1, sizeof(struct stream));
    s->type = input_type_file;

    FILE *fp = fopen(path, "r");
    if (!fp)
        errx(EXIT_FAILURE, "Error : stream_open_file - file pointer NULL");
    s->fp = fp;
    return s;
}

struct stream *stream_open_stdin(void)
{
    struct stream *s = calloc(1, sizeof(struct stream));
    s->type = input_type_stdin;
    s->fp = stdin;
    return s;
}

void stream_get_line(struct stream *s)
{
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread = getline(&line, &line_len, s->fp);

    if (nread <= 0)
    {
        free(line);
        return;
    }

    s->buffer = my_str_cat(s->buffer, line, nread);
    // s -> buffer = my_str_cat(s -> buffer, line, line_len);

    // printf("buffer_size = %lu\n",  s -> buffer_size);
    s->buffer_size += nread;
    free(line);
}

char stream_peek(struct stream *s)
{
    if (!s)
        errx(EXIT_FAILURE, "Error : stream_look - stream is NULL");

    if (s->is_at_end)
        return EOF;

    // <= to >=
    // here extra getline

    if (s->current >= s->buffer_size)
        stream_get_line(s);

    if (s->current < s->buffer_size)
        return s->buffer[s->current];

    return EOF;
}

char stream_advance(struct stream *s)
{
    if (!s)
        errx(EXIT_FAILURE, "Error : stream_advance - stream is NULL");

    if (s->is_at_end)
        return EOF;

    /*
     * if we're at the last character in our buffer,
     * read more data
     */
    // removed stream -> current + 1
    if (s->current >= s->buffer_size)
    {
        stream_get_line(s);
    }

    if (s->current < s->buffer_size)
    {
        /*
         * get the current character
         * in the buffer and increment the current index
         */
        char c = s->buffer[s->current];
        s->current += 1;

        if (c == EOF)
        {
            s->is_at_end = true;
        }
        return c;
    }

    return EOF;
}

void stream_go_back(struct stream *s)
{
    if (s->current > 0)
        s->current -= 1;
}

void stream_free(struct stream *s)
{
    if (!s)
        return;

    if (s->buffer)
        free(s->buffer);

    fclose(s->fp);
    free(s);
}

#define STREAM_MAIN
#ifndef STREAM_MAIN

int main(void)
{
    struct stream *s = stream_open_stdin();

    char c;
    while (true)
    {
        char expected = stream_peek(s);
        c = stream_advance(s);
        printf("\nexepected char = %c\n got = %c\n", expected,
               c == EOF ? '%' : c);
        if (c == EOF)
            break;
    }

    stream_free(s);
    return 0;
}
#endif /* ! STREAM_MAIN */
