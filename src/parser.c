#include "include/parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/error.h"
#include "include/instruction.h"
#include "include/lexer.h"
#include "include/operand.h"
#include "include/token.h"
#include "include/value.h"
#include "include/variable.h"

/*****************************************************************************
 * parser_init: Creates a new parser struct, initialise the struct           *
 *              variables and returns a pointer to said parser.              *
 *****************************************************************************/
parser* parser_init (lexer* lex) {
    parser* parser = malloc(sizeof(parser));
    if (!parser) return NULL;
    parser->lex = lex;
    parser->instr_buf_index = 0;
    parser->instr_count = 0;
    parser->label_buf_index = 0;
    parser->label_count = 0;
    parser->var_buf_index = 0;
    parser->var_count = 0;
    parser->current_tok = parser->lex->tok_buf[0];
    return parser;
}

/*****************************************************************************
 * prev_tok: Returns a pointer to the token that is n indexes prior in the   *
 *           given parser's lexer's token buffer.                            *
 *****************************************************************************/
token* prev_tok (parser* parser, const int num) {
    return parser->lex->tok_buf[parser->lex->tok_buf_index - num];
}

/*****************************************************************************
 * prev_var: Returns a pointer to the variable that is n indexes prior in    *
 *           the given parser's variable buffer.                             *
 *****************************************************************************/
variable* prev_var (parser* parser, const size_t num) {
    return parser->var_buf[parser->var_buf_index - num];
}

/*****************************************************************************
 * prev_instr: Returns a pointer to the instruction that is n indexes prior  *
 *             in the given parser's instruction buffer.                     *
 *****************************************************************************/
instruction* prev_instr (parser* parser, const size_t num){
    return parser->instr_buf[parser->instr_buf_index - num];
}

/*****************************************************************************
 * next_tok: Return a pointer to the next token in the the given parser's    *
 *           lexer's token buffer. Returns null if the token buffer index    *
 *           is greater then the total token_count.                          *
 *****************************************************************************/
token* next_tok (parser* parser) {
    parser->lex->tok_buf_index++;
    if(parser->lex->tok_buf_index > parser->lex->tok_count) return NULL;
    return parser->lex->tok_buf[parser->lex->tok_buf_index];
}

/*****************************************************************************
 * parser_add_var: Creates a new variable* pointer using the given arguments *
 *                 and inserts it into the given parser's variable buffer.   *
 *****************************************************************************/
void parser_add_var (parser* parser, variable_type type, char* var_name) {
    variable* var = new_variable(type, var_name);
    parser->var_buf[parser->var_buf_index] = var;
    parser->var_buf_index++;
    parser->var_count++;
}

/*****************************************************************************
 * parser_add_instruction: Creates a new instruction* pointer using the      *
 *                         given arguments and inserts it into the given     *
 *                         parser's instruction buffer.                      *
 *****************************************************************************/
void parser_add_instruction (parser* parser, char* opcode) {
    instruction* instr = new_instruction(opcode);
    parser->instr_buf[parser->instr_buf_index] =  instr;
    parser->instr_buf_index++;
}

/*****************************************************************************
 * parser_add_label: Creates a new label* pointer using the given arguments  *
 *                   and inserts it into the given parser's label buffer.    *
 *****************************************************************************/
void parser_add_label (parser* parser, char* label_name) {
    label* lab = new_label(parser->instr_buf_index, label_name);
    parser->label_buf[parser->label_buf_index] = lab;
    parser->label_buf_index++;
}

/*****************************************************************************
 * str_toupper: Capitalizes every character in a given string.               *
 *****************************************************************************/
void str_toupper (char* str) {
    size_t i = 0;
    while (str[i]) {str[i] = toupper(str[i]); i++;}
}

/*****************************************************************************
 * accept: Returns 0 if there is no current token in the given parser, or if *
 *         the current token's type does not match the given token_type.     *
 *         If the token_types match the parser's current_tok is updated and  *
 *         1 is returned.                                                    *
 *****************************************************************************/
int accept (parser* parser, const token_type t) {
    if (!parser->current_tok) return 0;
    if (parser->current_tok->type == t) {
        parser->current_tok = next_tok(parser);
        return 1;
    }
    return 0;
}

/*****************************************************************************
 * expect: Returns 1 if the given token_type is accpeted, or throws an       *
 *         unexpected symbol error using the provided error code.            *
 *****************************************************************************/
int expect (parser* parser, const token_type t, int error_code) {
    if (accept(parser, t)) return 1;
    throw_unexpected_symbol_error(parser, error_code);
    exit(1);
}

/*****************************************************************************
 * parser_parse_program: Parses an entire MIPS program. The .text section is *
 *                       mandatory but the .data section is optional and may *
 *                       come before or after the .text section.             *
 *                                                                           *
 * <program> ::= <text_section>                                              *
 *            |  <data_section><text_section>                                *
 *            |  <text_section><data_section>                                *
 *****************************************************************************/
void parser_parse_program (parser* parser) {
    if (accept(parser, TOK_FULLSTOP)) {
        if (strcmp(parser->current_tok->val, "data") == 0) {
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_data_section(parser);
            parse_text_section(parser);
        } else if (strcmp(parser->current_tok->val, "text") == 0) {
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_text_section(parser);
            parse_data_section(parser);
        } else throw_unexpected_symbol_error(parser, ERROR_TEXT_OR_DATA);
        if (parser->lex->tok_buf_index>parser->lex->tok_count)
            throw_unexpected_symbol_error(parser, ERROR_EOF);
    } else throw_unexpected_symbol_error(parser, ERROR_TEXT_OR_DATA);
}

/*****************************************************************************
 * parser_reset_buf_indexes: Sets all the total count variables of the       *
 *                           provided parser struct then resets all the      *
 *                           buffer indexes.                                 *
 *****************************************************************************/
void parser_reset_buf_indexes (parser* parser) {
    parser->var_count = parser->var_buf_index;
    parser->label_count = parser->label_buf_index;
    parser->instr_count = parser->instr_buf_index;
    parser->instr_buf_index = 0;
    parser->label_buf_index = 0;
    parser->var_buf_index = 0;
}

/*****************************************************************************
 * free_labels: Frees the memory used by all the labels in the given         *
 *              parser's label buffer.                                       *
 *****************************************************************************/
void free_labels (parser* parser) {
    for (size_t i = 0;i<parser->label_count;i++) 
        free(parser->label_buf[i]);
}

/*****************************************************************************
 * free_instrs: Frees the memory used by all the instructions in the given   *
 *              parser's instruction buffer. Also frees the memory used by   *
 *              all the operands within each of said instructions.           *
 *****************************************************************************/
void free_instrs (parser* parser) {
    for (size_t i = 0; i < parser->instr_count; i++) {
        free(parser->instr_buf[i]->operands[0]);
        free(parser->instr_buf[i]->operands[1]);
        free(parser->instr_buf[i]->operands[2]);
        free(parser->instr_buf[i]->opcode);
        free(parser->instr_buf[i]);
    }
}

/*****************************************************************************
 * free_variables: Frees the memory used by all the variables in the given   *
 *                 parser's variable buffer. Also frees the memory used by   *
 *                 all of the values within each of said variables.          *
 *****************************************************************************/
void free_variables (parser* parser) {
    for (size_t i=0;i<parser->var_count;i++) {
        for (size_t j=0;j<parser->var_buf[i]->value_count;j++)
            free(parser->var_buf[i]->value_buf[j]);
        free(parser->var_buf[i]);
    }
}

/*****************************************************************************
 * parser_free_all: Frees all label, instructions, and variables stored      *
 *                  within the provided parser struct.                       *
 *****************************************************************************/
void parser_free (parser* parser) {
    free_labels(parser);
    free_instrs(parser);
    free_variables(parser);
}