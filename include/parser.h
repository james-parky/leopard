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
} parser;

/*****************************************************************************
 * parser_init: Creates a new parser struct, initialise the struct           *
 *              variables and returns a pointer to said parser.              * 
 *****************************************************************************/
parser* parser_init(lexer* lex);

/*****************************************************************************
 * parser_parse_program: Analyzes the tokens in the given parse struct and   *
 *                       parses them following the MIPS32 assembly code      *
 *                       specification.                                      *
 *****************************************************************************/
void parser_parse_program(parser* parser);

/*****************************************************************************
 * parser_reset_buf_indexes: Resets the buffer indexes for the variable,     *
 *                           instruction and label buffers of the given      *
 *                           parser struct.                                  *
 *****************************************************************************/
void parser_reset_buf_indexes(parser* parser);

#endif