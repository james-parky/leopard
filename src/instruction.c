#include "instruction.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "operand.h"

instruction* new_instruction(char* opcode_str) {
    instruction* instr = malloc(sizeof(instruction));
    instr->opcode = opcode_str;
    for(size_t i = 0; i < 3; i++)
        instr->operands[i] = malloc(sizeof(operand*));
    instr->operands[2] =  NULL;
    return instr;
}

void instruction_add_operand(instruction* instr, operand_type type,
                                char* val) {
    operand* op = new_operand(type, val);
    instr->operands[instr->operand_buf_index] = op;
    instr->operand_buf_index++;
}