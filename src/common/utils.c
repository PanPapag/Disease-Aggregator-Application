#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/common/macros.h"
#include "../../includes/common/utils.h"

uint8_t string_to_int64(char* value, int64_t* value_out) {
    char* valid;
    *value_out = strtoll(value, &valid, 10);
    if (*valid != '\0') return 0;
    return 1;
}
