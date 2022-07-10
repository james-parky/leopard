#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "value.h"

#define BUF_MAX 255

typedef enum {
    VAR_WORD,
    VAR_HALF,
    VAR_BYTE,
    VAR_SPACE,
    VAR_ASCII,
    VAR_ASCIIZ
} variable_type;

typedef struct {
    variable_type type;
    char* name;
    value* value_buf[BUF_MAX];
    size_t value_buf_index;
    size_t value_count;
} variable;

variable* new_variable(variable_type type, char* name);

void variable_add_value(variable* var, value_type type, char* value);

#endif