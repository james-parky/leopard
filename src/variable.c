#include "value.h"
#include "variable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

variable* new_variable(variable_type type, char* name){
    variable* var = malloc(sizeof(variable));
    if(var == NULL) printf("not allocated\n");
    // var->type = type;
    // printf("%zu\n",len);
    // var->name = name;
    // memcpy(var->values, values, sizeof(value*) * len+1);
    // var->values[len]=NULL;
    var->type = type;
    var->name = name;
    var->value_buf_index = 0;
    var->value_count = 0;
    for(size_t i=0;i<BUF_MAX;i++) var->value_buf[i] = malloc(sizeof(value*));
    return var;
}

void variable_add_value(variable* var, value_type type, char* val){
    value* value = new_value(type, val);
    var->value_buf[var->value_buf_index] = value;
    var->value_buf_index++;
    var->value_count++;
}