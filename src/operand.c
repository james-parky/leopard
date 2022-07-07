#include "operand.h"

#include <stdlib.h>

operand* new_operand(operand_type type, char* val){
    operand* op = malloc(sizeof(operand));
    op->type = type;
    op->val = val;
    return op;
}