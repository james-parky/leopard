#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "value.h"

#define BUF_MAX 1000

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
    value* values[BUF_MAX];
} variable;

variable* new_variable(variable_type type, char* name, value* values[]);

#endif