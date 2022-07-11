#ifndef __OPERAND_H__
#define __OPERAND_H__

typedef enum {
    OP_REG,
    OP_DEC_NUM,
    OP_NEG_DEC_NUM,
    OP_HEX_NUM,
    OP_IDN,
    OP_INX_REG,
    OP_IDN_INX_REG,
    OP_IDN_DEC_NUM,
    OP_IDN_PLUS_INX_REG
} operand_type;

typedef struct {
    operand_type type;
    char* val;
} operand;

/*****************************************************************************
 * new_operand: Creates a new operand struct, using the given arguments and  *
 *              returns a pointer to said operand.                           * 
 *****************************************************************************/
operand* new_operand(operand_type type, char* val);

#endif