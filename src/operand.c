#include "operand.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

operand* new_operand(operand_type type, char* val) {
    operand* op = malloc(sizeof(operand));
    op->type = type;
    // op->val = malloc(sizeof(char) * strlen(val) + 1);
    // strncpy(op->val, val, strlen(val));
    // op->val[strlen(val)] = '\0';
    op->val = strdup(val);
    return op;
}