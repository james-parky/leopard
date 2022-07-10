#include "instruction.h"
#include <stdlib.h>
#include <string.h>

#include "operand.h"
#include <stdio.h>

instruction* new_instruction(char* opcode_str, operand* operands[], size_t len){
    instruction* instr = malloc(sizeof(instruction));
    instr->opcode = opcode_str;
    for(size_t i=0;i<BUF_MAX;i++) instr->operands[i] = malloc(sizeof(operand*));
    memcpy(instr->operands, operands, sizeof(operand*) * len +1);
    instr->operands[len] =  NULL;
    return instr;
}
