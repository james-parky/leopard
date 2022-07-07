#include "instruction.h"
#include <stdlib.h>
#include <string.h>

#include "operand.h"

instruction* new_instruction(char* opcode_str, operand* operands[]){
    instruction* instr = malloc(sizeof(instruction));
    instr->opcode = opcode_str;
    memcpy(&instr->operands, &operands, sizeof(operand*));
    return instr;
}