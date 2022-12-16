#include "log.h"
#include <stdio.h>
#include <stdbool.h>

static Logger default_logger = {NULL, LL_WARN, 1, 1, 1, 1, 1};