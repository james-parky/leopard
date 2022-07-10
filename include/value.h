#ifndef __VALUE_H__
#define __VALUE_H__

#include "token.h"

typedef enum {
    VAL_DEC_NUM,
    VAL_NEG_DEC_NUM,
    VAL_HEX_NUM,
    VAL_IDENTIFIER,
    VAL_ASCII,
    VAL_ASCIIZ
} value_type;

typedef struct {
    value_type type;
    char* val;
} value;

value* new_value(value_type type, char* val);
char* get_val(value* value);
#endif