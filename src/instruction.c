#include "include/instruction.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/operand.h"

instruction* new_instruction(char* opcode_str) {
    instruction* instr = malloc(sizeof(instruction));
    instr->opcode = strdup(opcode_str);
    return instr;
}

void instruction_add_operand(instruction* instr, operand_type type,
                                char* val) {
    operand* op = new_operand(type, val);
    instr->operands[instr->operand_buf_index] = op;
    instr->operand_buf_index++;
}