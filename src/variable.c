#include "value.h"
#include "variable.h"
#include <stdlib.h>
#include <string.h>

variable* new_variable(variable_type type, char* name, value* values[]){
    variable* var = malloc(sizeof(variable));
    var->type = type;
    var->name = name;
    memcpy(&var->values, &values, sizeof(value*));
    return var;
}