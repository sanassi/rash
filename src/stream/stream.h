#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../utils/utils.h"

enum stream_input_type
{
    input_type_string,
    input_type_stdin,
    input_type_file,
};

struct stream
{
    FILE *fp;
    enum stream_input_type type;

    char *buffer;
    ssize_t current;
    ssize_t buffer_size;

    bool is_at_end;
};

struct stream *stream_open_string(char *string);
struct stream *stream_open_file(char *path);
struct stream *stream_open_stdin(void);
char stream_peek(struct stream *s);
char stream_advance(struct stream *s);
void stream_free(struct stream *s);
void stream_go_back(struct stream *s);
#endif /* ! STREAM_H */
