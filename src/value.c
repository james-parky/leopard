#include "value.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

value* new_value(value_type type, char* val){
    value* value = malloc(sizeof(value));
    value->type = type;
    value->val = val;
    return value;
}