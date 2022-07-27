#ifndef __PARSER_H__
#define __PARSER_H__

#include "instruction.h"
#include "label.h"
#include "lexer.h"
#include "token.h"
#include "variable.h"

typedef struct {
    lexer* lex;

    instruction* instr_buf[BUF_MAX];
    label* label_buf[BUF_MAX];
    variable* var_buf[BUF_MAX];
    char* entry_point;

    size_t instr_buf_index, instr_count;
    size_t label_buf_index, label_count;
    size_t var_buf_index, var_count;

    token* current_tok;
    variable* current_var;
    instruction* current_instr;
} parser;

/*****************************************************************************
 * parser_init: Creates a new parser struct, initialise the struct           *
 *              variables and returns a pointer to said parser.              *
 *****************************************************************************/
parser* parser_init(lexer* lex);

/*****************************************************************************
 * parser_reset_buf_indexes: Resets the buffer indexes for the variable,     *
 *                           instruction and label buffers of the given      *
 *                           parser struct.                                  *
 *****************************************************************************/
void parser_reset_buf_indexes(parser* parser);

/*****************************************************************************
 * parser_reset_buf_indexes: Sets all the total count variables of the       *
 *                           provided parser struct then resets all the      *
 *                           buffer indexes.                                 *
 *****************************************************************************/
void parser_reset_buf_indexes(parser* parser);

/*****************************************************************************
 * parser_free: Free all labels, variables, and instructions stored          *
 *              within the given parser's buffers.                       *
 *****************************************************************************/
void parser_free(parser* parser);

/*****************************************************************************
 * accept: Returns 0 if there is no current token in the given parser, or if *
 *         the current token's type does not match the given token_type.     *
 *         If the token_types match the parser's current_tok is updated and  *
 *         1 is returned.                                                    *
 *****************************************************************************/
int accept(parser* parser, const token_type t);

/*****************************************************************************
 * expect: Returns 1 if the given token_type is accpeted, or throws an       *
 *         unexpected symbol error using the provided error code.            *
 *****************************************************************************/
int expect(parser* parser, const token_type t, int error_code);

/*****************************************************************************
 * next_tok: Return a pointer to the next token in the the given parser's    *
 *           lexer's token buffer. Returns null if the token buffer index    *
 *           is greater then the total token_count.                          *
 *****************************************************************************/
token* next_tok(parser* parser);

/*****************************************************************************
 * prev_tok: Returns a pointer to the token that is n indexes prior in the   *
 *           given parser's lexer's token buffer.                            *
 *****************************************************************************/
token* prev_tok(parser* parser, const int num);

/*****************************************************************************
 * prev_var: Returns a pointer to the variable that is n indexes prior in    *
 *           the given parser's variable buffer.                             *
 *****************************************************************************/
variable* prev_var(parser* parser, const size_t num);

/*****************************************************************************
 * prev_instr: Returns a pointer to the instruction that is n indexes prior  *
 *             in the given parser's instruction buffer.                     *
 *****************************************************************************/
instruction* prev_instr(parser* parser, const size_t num);

/*****************************************************************************
 * parser_add_var: Creates a new variable* pointer using the given arguments *
 *                 and inserts it into the given parser's variable buffer.   *
 *****************************************************************************/
void parser_add_var(parser* parser, variable_type type, char* var_name);

/*****************************************************************************
 * parser_add_instruction: Creates a new instruction* pointer using the      *
 *                         given arguments and inserts it into the given     *
 *                         parser's instruction buffer.                      *
 *****************************************************************************/
void parser_add_instruction(parser* parser, char* opcode);

/*****************************************************************************
 * parser_add_label: Creates a new label* pointer using the given arguments  *
 *                   and inserts it into the given parser's label buffer.    *
 *****************************************************************************/
void parser_add_label(parser* parser, char* label_name);

/*****************************************************************************
 * str_toupper: Capitalizes every character in a given string.               *
 *****************************************************************************/
void str_toupper(char* str);

/*****************************************************************************
 * parse_negative_data_value: Parses either a single negative number or MIPS *
 *                            style, colon-multiple-initialization list      *
 *                            pattern declaration.                           *
 *                                                                           *
 * <neg_value> ::= "-"<dec_num>                                              *
 *              |  "-"<dec_num>":"<dec_num>                                  *
 *              |  "-"<dec_num>":"<hex_num>                                  *
 *****************************************************************************/
void parse_negative_data_value(parser* parser);

/*****************************************************************************
 * parse_dec_number_data_value: Parses either a single base 10 number or     *
 *                              a MIPS style, colon-multiple-initialization  *
 *                              list pattern declaration.                    *
 *                                                                           *
 * <dec_value> ::= <dec_num>                                                 *
 *              |  <dec_num>":"<dec_num>                                     *
 *              |  <dec_num>":"<hex_num>                                     *
 *****************************************************************************/
void parse_dec_number_data_value(parser* parser);

/*****************************************************************************
 * parse_hex_number_data_value: Parses either a single base 16 number or     *
 *                              a MIPS style, colon-multiple-initialization  *
 *                              list pattern declaration.                    *
 *                                                                           *
 * <hex_value> ::= <hex_num>                                                 *
 *              |  <hex_num>":"<dec_num>                                     *
 *              |  <hex_num>":"<hex_num>                                     *
 *****************************************************************************/
void parse_hex_number_data_value(parser* parser);

/*****************************************************************************
 * parse_data_value: Parse either a positive / negative number of base 10 or *
 *                   or base 16, or an identifier. Throws an unexpected      *
 *                   symbol is neither is found.                             *
 *                                                                           *
 * <data_value> ::= <neg_value> | <dec_value> | <hex_value>                  *
 *****************************************************************************/
void parse_data_value(parser* parser);

/*****************************************************************************
 * parse_data_list: Continuously parses data values as long as commans are   *
 *                  also parsed in-between.                                  *
 *                                                                           *
 * <data_list> ::= <data_value> | <data_value>","<data_list>                 *
 *****************************************************************************/
void parse_data_list(parser* parser);

/*****************************************************************************
 * parse_variable_declaration: Parses a MIPS data type followed by a value   *
 *                             or data_list appropriate to that data type.   *
 *                                                                           *
 * <data_type> ::= "ascii" | "asciiz" | "word" | "half" | "byte" | "space"   *
 * <var_dec> ::= "."<data_type><data_list>                                   *
 *****************************************************************************/
void parse_variable_declaration(parser* parser, char* var_name);

/*****************************************************************************
 * parse_data_section: Parses the full .data section of a MIPS program.      *
 *                                                                           *
 * <var_decs> ::= <var_dec> | <var_dec><var_decs>                            *
 * <data_section> ::= ".data"<var_decs>                                      *
 *****************************************************************************/
void parse_data_section(parser* parser);

/*****************************************************************************
 * parse_register_name: Parses a register within MIPS.                       *
 *                                                                           *
 * <num_reg> ::= "0" | "1" | ... | "32"                                      *
 * <name_reg> ::= "zero" | "at" | "v0" | "v1" | "a0" | "a1" | "a2" | "a3"    *
 *             |  "t0" | "t1" | "t2" | "t3" | "t4" | "t5" | "t6" | "t7"      *
 *             |  "s0" | "s1" | "s2" | "s3" | "s4" | "s5" | "s6" | "s7"      *
 *             |  "t8" | "t9" | "k0" | "k1" | "gp" | "sp" | "fp" | "ra"      *
 * <register> ::= "$"<num_reg> | "$"<name_reg>                               *
 *****************************************************************************/
void parse_register_name(parser* parser);

/*****************************************************************************
 * parse_operand: Parses a valid operand within MIPS - which can be either   *
 *                a register, a register with an offsetting, value, a label  *
 *                or a variable name.                                        *
 *                                                                           *
 * <offset_register> ::= "("<register>")"                                    *
 *                    |  <dec_num>"("<register>")"                           *
 *                    |  <hex_num>"("<register>")"                           *
 *                    |  <string>"+""("<register>")"                         *
 *                    |  <string>"("<register>")"                            *
 * <operand> ::= <register> | <offset_register> | <data_value>               *
 *****************************************************************************/
void parse_operand(parser* parser);

/*****************************************************************************
 * parse_operand_list: Continuously parses operands as long as there are     *
 *                     commas in-between.                                    *
 *                                                                           *
 * <operand_list> ::= <operand> | <operand>","<operand_list>                 *
 *****************************************************************************/
void parse_operand_list(parser* parser);

/*****************************************************************************
 * parse_instruction: Parses a full instruction call within MIPS - which     *
 *                    consists of an opcode and a set of operands. Or just   *
 *                    an opcode in the case of a syscall.                    *
 *                                                                           *
 * <instruction> ::= <opcode> | <opcode><operand_list>                       *
 *****************************************************************************/
void parse_instruction(parser* parser);

/*****************************************************************************
 * parse_text_section: Parses the entire text section of a MIPS program.     *
 *                     This consists of an optional entry point declaration  *
 *                     followed by some number of instructions and labels.   *
 *                                                                           *
 * <label> ::= <string>":"                                                   *
 * <text_statement> ::= <label> | <instruction>                              *
 * <text_body> ::= <text_statement> | <text_statement><text_body>            *
 * <text_section> ::= ".text"".globl"<string><text_body>                     *
 *                 |  ".text"<text_body>                                     *
 *****************************************************************************/
void parse_text_section(parser* parser);

/*****************************************************************************
 * parser_parse_program: Parses an entire MIPS program. The .text section is *
 *                       mandatory but the .data section is optional and may *
 *                       come before or after the .text section.             *
 *                                                                           *
 * <program> ::= <text_section>                                              *
 *            |  <data_section><text_section>                                *
 *            |  <text_section><data_section>                                *
 *****************************************************************************/
void parser_parse_program(parser* parser);

#endif