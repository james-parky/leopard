#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include <stddef.h>

#include "value.h"

#define VALUE_BUF_MAX 32

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
    value* value_buf[VALUE_BUF_MAX];
    size_t value_buf_index;
    size_t value_count;
} variable;

/*****************************************************************************
 * new_variable: Creates a new variable struct using the given arguments     *
 *               and returns a pointer to said variable.                     *
 *****************************************************************************/
variable* new_variable(variable_type type, char* name);

/*****************************************************************************
 * variable_add_value: Creates a new value struct using the given arguments  *
 *                     inserts it into the value buffer of the given         *
 *                     variable struct.                                      *
 *****************************************************************************/
void variable_add_value(variable* var, value_type type, char* value);

#endif