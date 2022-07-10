#include "value.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

value* new_value(value_type type, char* val){
    value* value = malloc(sizeof(value));
    value->type = type;
    //value->val = val;
    value->val = malloc(sizeof(val));
    memcpy(value->val, val, strlen(val));
    if(value->val == NULL) printf("cant allocate\n");
    return value;
}

char* get_val(value* value){
    return value->val;
}