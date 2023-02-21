#ifndef ECHO_H
#define ECHO_H

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/utils.h"
#include "bool.h"
#include <unistd.h>

#include "../../env/env.h"

int echo(struct vector *args, struct env *env);

#endif /* ! ECHO_H */
