#ifndef __VALUE_H__
#define __VALUE_H__

#include "token.h"


typedef enum {
    VAL_DEC_NUM,
    VAL_NEG_DEC_NUM,
    VAL_HEX_NUM,
    VAL_IDENTIFIER,
    VAL_ASCII,
    VAL_ASCIIZ,
    VAL_CHAR
} value_type;

typedef struct {
    value_type type;
    char* val;
} value;

/*****************************************************************************
 * new_value: Creates a new value struct using the given arguments and       *
 *            returns a pointer to said value.                               *
 *****************************************************************************/
value* new_value(value_type type, char* val);

#endif