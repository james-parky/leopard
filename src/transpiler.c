#include "include/transpiler.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/error.h"
#include "include/instruction.h"
#include "include/lexer.h"
#include "include/operand.h"
#include "include/parser.h"
#include "include/value.h"
#include "include/variable.h"

#define BOLD "\x1B[1m"
#define GREEN "\033[32;1m"
#define RED "\033[31;1m"
#define RESET "\033[0m"

#define ARM_DATA_TYPES ".word", ".hword", ".byte", ".space", ".ascii", ".asciz"

transpiler* transpiler_init(parser* parser, char* in_file_name, FILE* out,
                            char* out_file_name){
    transpiler* transp = malloc(sizeof(transpiler));
    transp->in_file_name = in_file_name;
    transp->out_file_name = out_file_name;
    transp->out = out;
    transp->parser = parser;
    return transp;
}

// int is_mips_name_reg(char* reg) {
//     const char* reg_names[32] = {"zero", "at", "v0", "v1", "a0", "a1",
//                                         "a2", "a3", "t0", "t1", "t2", "t3",
//                                         "t4", "t5", "t6", "t7", "s0", "s1",
//                                         "s2", "s3", "s4", "s5", "s6", "s7",
//                                         "t8", "t9", "k0", "k1", "gp", "sp",
//                                         "fp", "ra"};
//     for (short i = 0; i < 32; i++) 
//         if(strcmp(reg, reg_names[i]) == 0) return i;
    
//     return -1;
// }

void replace_registers (transpiler* transp, instruction* instr) {
    /*
    a0 - x0
    a1 - x1
    a2 - x2
    a3 - x3

    v0 - dependant on syscall
    v1 - dependant on syscall
    v2 - dependant on syscall

    ? - x6
    ? - x7

    t0 - x8
    ...
    t7 - x18

    s0 - x19
    ...
    s1 - x28
    
    fp - x29
    ra - 30
    */
}

void write_ascii_var_dec(transpiler* transp, variable* var) {
    printf("ascii var val: %s\n", var->value_buf[0]->val);

    char* var_type = malloc(sizeof(char) * 7);
    if (var->type == VAR_ASCII) strncpy(var_type, ".ascii", 6);
    if (var->type == VAR_ASCIIZ) strncpy(var_type, ".asciz", 6);
    var_type[6] = '\0';

    char* val = malloc(sizeof(char) * strlen(var->value_buf[0]->val) + 1);
    strcpy(val, var->value_buf[0]->val);
    val[strlen(var->value_buf[0]->val)] = '\0';
    fprintf(transp->out, "%s: %s\t", var->name, var_type);
    fprintf(transp->out, "\"%s\"\n", val);
    fprintf(transp->out, "%s_len = . - %s\n", var->name, var->name);
    free(val);
    free(var_type);
}

void write_space_var_dec(transpiler* transp, variable* var) {
    fprintf(transp->out, "%s: .space\t%s\n", var->name, 
            var->value_buf[0]->val);
}

void write_num_var_dec(transpiler* transp, variable* var) {
    const char* arm64_data_types[6] = {ARM_DATA_TYPES};
    for(size_t j = 0; j < var->value_count; j++){
        fprintf(transp->out, "%s_%zu: %s\t", var->name, j,
                arm64_data_types[var->type]);
        char val_prefix[3];
        switch(var->value_buf[j]->type) {
            case VAL_HEX_NUM: strcpy(val_prefix, "0x\0"); break;
            case VAL_NEG_DEC_NUM: strcpy(val_prefix, "-\0"); break;
            default: break;
        }
        fprintf(transp->out, "%s%s\n", val_prefix, var->value_buf[j]->val);
    }
}

void write_var_dec (transpiler* transp, variable* var) {
    fprintf(transp->out, "# %s\n", var->name);
    switch(var->type){
        case VAR_ASCII:
        case VAR_ASCIIZ:
            write_ascii_var_dec(transp, var);
            break;
         case VAR_SPACE:
            write_space_var_dec(transp, var);
            break;
        case VAR_WORD:
        case VAR_BYTE:
        case VAR_HALF:
            write_num_var_dec(transp, var);
            break;
        default:
            break;
    }
    fprintf(transp->out, "\n");
}

void write_label(transpiler* transp, label* lab){
    fprintf(transp->out, "%s:\n", lab->val);
}

void write_num (transpiler* transp, char* val) {
    fprintf(transp->out, "#%d", atoi(val));
}

void write_reg (transpiler* transp, char* val) {
    int reg_num = atoi(val+1);
    if(*val=='s') reg_num+=19;
    if(*val=='t') reg_num+=8;
    fprintf(transp->out, "x%d", reg_num);
}

void write_idn (transpiler* transp, char* val) {
    fprintf(transp->out, "%s", val);
}

void write_inx_reg (transpiler* transp, char* val) {
    size_t dec_len = 0;
    while (isdigit(val[dec_len])) dec_len++;
    char dec[dec_len + 1];
    strncpy(dec, val, dec_len);
    dec[dec_len] = '\0';
    fprintf(transp->out, "[");
    write_reg(transp, val + dec_len);
    fprintf(transp->out, ", #%s]", dec);
}

void write_idn_plus_inx_reg (transpiler* transp, operand* op) {

}

void write_idn_dec_num (transpiler* transp, operand* op) {

}

void write_idn_inx_reg (transpiler* transp, operand* op) {

}

void write_operands (transpiler* transp, instruction* instr) {
  for (size_t i = 0; i < instr->operand_buf_index; i++) {
        operand* op = instr->operands[i];
        switch (op->type) {
            case OP_REG:
                write_reg(transp, op->val);
                break;
            case OP_DEC_NUM:
            case OP_NEG_DEC_NUM:
            case OP_HEX_NUM:
                write_num(transp, op->val);
                break;
            case OP_IDN:
                write_idn(transp, op->val);
                break;
            case OP_INX_REG:
                write_inx_reg(transp, op->val);
                break;
        }
        if (i != 2) fprintf(transp->out, ", ");
        else fprintf(transp->out, "\n");
    }
}


/*
    OP_IDN_INX_REG,
    OP_IDN_DEC_NUM,
    OP_IDN_PLUS_INX_REG
*/

void write_add (transpiler* transp, instruction* instr) {
    fprintf(transp->out, "\tadd\t");
    write_operands(transp, instr);
    fprintf(transp->out, "\n");
}

void write_and (transpiler* transp, instruction* instr) {
    fprintf(transp->out, "\tand\t");
    write_operands(transp, instr);
    fprintf(transp->out, "\n");
}

// void write_beq (transpiler* transp, instruction* instr) {
//     fprintf(transp->out, "\tcbz\t");
//     write_reg(transp, instr->operands[0]->val);
//     fprintf(transp->out, ", ");
//     write_identifier(transp, instr->operands[1]->val);
//     fprintf(transp->out, "\n");
// }


void write_abs (transpiler* transp, instruction* instr) {
    printf("Writing abs\n");
    instruction* sra = new_instruction("SRA");
    instruction_add_operand(sra, OP_REG, "x8");
    instruction_add_operand(sra, instr->operands[1]->type, instr->operands[1]->val);
    instruction_add_operand(sra, OP_HEX_NUM, "1f");
    //write_base_instr(transp, sra);

    printf("writing xor\n");
    instruction* xor = new_instruction("XOR");
    instruction_add_operand(xor, OP_REG, instr->operands[0]->val);
    instruction_add_operand(xor, OP_REG, "x8");
    instruction_add_operand(xor, OP_REG, instr->operands[1]->val);
    //write_base_instr(transp, xor);

    printf("writing subu\n");
    instruction* subu = new_instruction("SUBU");
    instruction_add_operand(subu, OP_REG, instr->operands[0]->val);
    instruction_add_operand(subu, OP_REG, instr->operands[0]->val);
    instruction_add_operand(subu, OP_REG, "x8");
    //write_base_instr(transp, subu);
}

int is_pseudo(instruction* instr) {
    return 0;
}

void write_pseudo_instr(transpiler* transp, instruction* instr) {
     if (strcmp(instr->opcode, "ABS") == 0) write_abs(transp, instr);
}

void write_base_instr(transpiler* transp, instruction* instr) {
    char* opcode = instr->opcode;
    char arm64_opcode[5];

    if (strcmp(opcode, "ADD") == 0) strcpy(arm64_opcode, "add");
    if (strcmp(opcode, "ADDU") == 0) strcpy(arm64_opcode, "add");
    if (strcmp(opcode, "ADDI") == 0) strcpy(arm64_opcode, "add");
    if (strcmp(opcode, "ADDIU") == 0) strcpy(arm64_opcode, "add");

    if (strcmp(opcode, "AND") == 0) strcpy(arm64_opcode, "and");    
    if (strcmp(opcode, "ANDI") == 0) strcpy(arm64_opcode, "and");
    if(strcmp(opcode, "ORI") == 0) strcpy(arm64_opcode, "and");

    // if(strcmp(opcode, "LUI") == 0) write_li(transp, instr);
    // if(strcmp(opcode, "NOR") == 0) 

    if(strcmp(opcode, "OR") == 0) strcpy(arm64_opcode, "orr");
    if(strcmp(opcode, "ORI") == 0) strcpy(arm64_opcode, "orr");

    if(strcmp(opcode, "LA") == 0) strcpy(arm64_opcode, "adr");
    if(strcmp(opcode, "LW") == 0) strcpy(arm64_opcode, "ldr");
    if(strcmp(opcode, "SW") == 0) strcpy(arm64_opcode, "str");

    // if(strcmp(opcode, "SLT") == 0) write_slt(transp, instr);
    // if(strcmp(opcode, "SLTI") == 0) write_slt(transp, instr);
    // if(strcmp(opcode, "SLTIU") == 0) write_slt(transp, instr);
    // if(strcmp(opcode, "SLTU") == 0) write_slt(transp, instr);

    if(strcmp(opcode, "SUB") == 0) strcpy(arm64_opcode, "sub");
    if(strcmp(opcode, "SUBU") == 0) strcpy(arm64_opcode, "sub");

    if(strcmp(opcode, "MOVE") == 0) strcpy(arm64_opcode, "mov");

    // if(strcmp(opcode, "XOR") == 0) write_xor(transp, instr);
    // if(strcmp(opcode, "XORI") == 0) write_xor(transp, instr);

    // if(strcmp(opcode, "SLL") == 0) write_sll(transp, instr);
    // if(strcmp(opcode, "SLLV") == 0) write_sll(transp, instr);

    // if(strcmp(opcode, "SRA") == 0) write_sra(transp, instr);
    // if(strcmp(opcode, "SRAV") == 0) write_sra(transp, instr);
    // if(strcmp(opcode, "SRL") == 0) write_sra(transp, instr);
    // if(strcmp(opcode, "SRLV") == 0) write_sra(transp, instr);

    // if(strcmp(opcode, "DIV") == 0) write_div(transp, instr);
    // if(strcmp(opcode, "DIVU") == 0) write_div(transp, instr);

    // if(strcmp(opcode, "MULT") == 0) write_mul(transp, instr);
    // if(strcmp(opcode, "MULTU") == 0) write_mul(transp, instr);

    // if(strcmp(opcode, "BEQ") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BGEZ") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BGEZAL") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BGTZ") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BLEZ") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BLTZ") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BLTZAL") == 0) write_branch(transp, instr);
    // if(strcmp(opcode, "BNE") == 0) write_branch(transp, instr);

    // if(strcmp(opcode, "BREAK") == 0) write_break(transp, instr);

    // if(strcmp(opcode, "J") == 0) write_jump(transp, instr);
    // if(strcmp(opcode, "JAL") == 0) write_jump(transp, instr);
    // if(strcmp(opcode, "JALR") == 0) write_jump(transp, instr);
    // if(strcmp(opcode, "JR") == 0) write_jump(transp, instr);

    // if(strcmp(opcode, "SYSCALL") == 0) write_svc(transp, instr);

    // if(strcmp(opcode, "LB") == 0)  write_load(transp, instr);
    // if(strcmp(opcode, "LBU") == 0)  write_load(transp, instr);
    // if(strcmp(opcode, "LH") == 0)  write_load(transp, instr);
    // if(strcmp(opcode, "LBU") == 0)  write_load(transp, instr);
    // if(strcmp(opcode, "LW") == 0)  write_load(transp, instr);

    // if(strcmp(opcode, "SB") == 0) write_store(transp, instr);
    // if(strcmp(opcode, "SH") == 0) write_store(transp, instr);
    // if(strcmp(opcode, "SW") == 0) write_store(transp, instr);

    fprintf(transp->out, "\t%s\t", arm64_opcode);
    write_operands(transp, instr);
}

void write_instr(transpiler* transp, instruction* instr) {
    replace_registers(transp, instr);
    if(is_pseudo(instr)) write_pseudo_instr(transp, instr);
    else write_base_instr(transp, instr);


    //if (strcmp(opcode, "XOR") == 0) write_xor(transp, instr);

    // if (strcmp(opcode, "BEQ") == 0) write_beq(transp, instr);
    // ===============


    // if(strcmp(opcode, "MFHI") == 0)
    // if(strcmp(opcode, "MFLO") == 0) 
    // if(strcmp(opcode, "MTHI") == 0) 
    // if(strcmp(opcode, "MTLO") == 0) 
    // if(strcmp(opcode, "MFC0") == 0) 
    // if(strcmp(opcode, "MTC0") == 0) 

}

void write_data_section (transpiler* transp) {
    fprintf(transp->out, ".data\n");
    for (size_t i = 0;i<transp->parser->var_count; i++) {
        variable* var = transp->parser->var_buf[i];
        write_var_dec(transp, var);
    }
}

void write_text_section (transpiler* transp) {
    // Change based on what entry point is, if there is code above where the entry point label is
    fprintf(transp->out, "\n.text\n.globl _start\n_start:\n");
    size_t current_lab_index = 0;
    for (size_t i = 0; i < transp->parser->instr_count; i++) {
        label* current_lab = transp->parser->label_buf[current_lab_index];
        if (current_lab->index == i) {
            write_label(transp, current_lab);
            current_lab_index++;
        }
        instruction* instr = transp->parser->instr_buf[i];
        write_instr(transp, instr);
    }
}

void transpiler_code_gen (transpiler* transp) {
    fprintf(transp->out, "# Code generated by leopard from '%s'\n",
            transp->in_file_name);
    write_data_section(transp);
    write_text_section(transp);
}

void transpile_file (FILE* in, char* in_file_name, FILE* out, 
                    char* out_file_name) {
    lexer* lex = lexer_init(in, in_file_name);
    lexer_lex(lex, 1);
    fclose(lex->file);

    parser* parser = parser_init(lex);
    parser_parse_program(parser);
    parser_reset_buf_indexes(parser);

    
    transpiler* transp = transpiler_init(parser, in_file_name, out, 
                                            out_file_name);
    transpiler_code_gen(transp);
    lexer_free(lex);
    parser_free(parser);
    free(lex);
    free(parser);
    free(transp);
}