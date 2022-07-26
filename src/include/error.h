#ifndef __ERROR_H__
#define __ERROR_H__

#include "instruction.h"
#include "parser.h"
#include "transpiler.h"

typedef enum {
    ERROR_NEG_NUM,
    ERROR_VAR_DEC,
    ERROR_NUM,
    ERROR_DATA,
    ERROR_LABEL,
    ERROR_REG,
    ERROR_RIGHT_PAREN,
    ERROR_DEC_NUM,
    ERROR_INSTR,
    ERROR_ENTRY,
    ERROR_LABEL_OR_INSTR,
    ERROR_TEXT,
    ERROR_TEXT_OR_DATA,
    ERROR_EOF,
    ERROR_VAR_TYPE,
    ERROR_GLOBL
} unexpected_symbol_error_code;

void throw_unexpected_symbol_error (parser* parser, const int error_code);

void throw_non_ascii_error (lexer* lex, size_t line_num, size_t col_num,
                            wchar_t wc);

void throw_invalid_reg_error (parser* parser, token* tok);
#endif