#ifndef CD_H
#define CD_H

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

#define CD_ERROR_EXIT -1

int cd(struct vector *args);

#endif /* ! CD_H */
