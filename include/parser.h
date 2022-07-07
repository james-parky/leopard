#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include "token.h"
#include "label.h"
#include "instruction.h"
#include "variable.h"

typedef struct {
    lexer* lex;
    instruction* instr_buf[BUF_MAX];
    label* label_buf[BUF_MAX];
    variable* var_buf[BUF_MAX];
    char* entry_point;
    size_t instr_buf_index, instr_count, label_buf_index, label_count, var_buf_index, var_count;
    token* current_tok;
} parser;

parser* parser_init(lexer* lex);
void parser_parse_program(parser* parser);

#endif