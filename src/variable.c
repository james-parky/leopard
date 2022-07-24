#include "include/variable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/value.h"

variable* new_variable(variable_type type, char* name){
    variable* var = malloc(sizeof(variable));
    var->type = type;
    var->name = name;
    var->value_buf_index = 0;
    var->value_count = 0;
    return var;
}

void variable_add_value(variable* var, value_type type, char* val){
    value* value = new_value(type, val);
    var->value_buf[var->value_buf_index] = value;
    var->value_buf_index++;
    var->value_count++;
}