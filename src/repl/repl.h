#ifndef REPL_H
#define REPL_H

#include "../execution/builtins/bool.h"
#include "../parser/ast_free.h"
#include "../parser/ast_print.h"
#include "../parser/parser.h"
#include "../lexer/lexer.h"
#include "../stream/stream.h"
#include "../execution/execution.h"
#include <bits/getopt_core.h>
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../env/env.h"
#include "../utils/utils.h"
#include "../args/args.h"

struct stream *stream_open(struct program_args *args);
int rash_repl(struct program_args *args, struct env *env);

#endif /* ! REPL_H */
