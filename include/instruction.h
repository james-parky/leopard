#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <stddef.h>

#include "operand.h"

#define BUF_MAX 255

typedef enum {
    ADD, ADDU, AND, NOR, OR, SLT, SLTU, SUB, SUBU, XOR, ADDI, ADDIU, ANDI, LUI,
    ORI, SLTI, SLTIU, XORI, SLL, SLLV, SRA, SRAV, SRL, SRLV, DIV, DIVU, MULT,
    MULTU, MFHI, MFLO, MTHI, MTLO, MTC0, MTC1, MTC1_D, MOVE, MFC0, MFC1,
    MFC1_D, BEG, BGEZ, BGEZAL, BEQZ, SUBI, SUBIU, BGTZ, BGT, BEQ, BLEZ, BLTZ,
    BLTZAL, BNE, J, BLE, BLT, BGE, JAL, JALR, JR, LB, LBU, LH, LHU, LW, LA, LI,
    SB, SH, SW, BREAK, SYSCALL
} opcode;

typedef struct {
    char* opcode;
    operand* operands[BUF_MAX];
    size_t operand_buf_index;
} instruction;

/*****************************************************************************
 * new_instruction: Creates a new instruction struct using the given         *
 *                  arguments and returns a pointer to said instruction.     *
 ****************************************************************************/
instruction* new_instruction(char* opcode_str, operand* operands[],
                                size_t len);
#endif