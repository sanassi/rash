#ifndef DOT_H
#define DOT_H

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../utils/vector/vector.h"
#include "../../utils/utils.h"
#include "../../env/env.h"

int dot(struct vector *args, struct env *env);

#endif /* ! DOT_H */
