#ifndef ECHO_H
#define ECHO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../env/env.h"
#include "../../utils/utils.h"
#include "bool.h"

int echo(struct vector *args, struct env *env);

#endif /* ! ECHO_H */
