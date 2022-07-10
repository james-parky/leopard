#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stddef.h>

typedef enum {  
    TOK_IDENTIFIER,
    TOK_STRING,
    TOK_COMMENT,

    TOK_L_PAREN,
    TOK_R_PAREN,
    TOK_DOLLAR_SIGN,
    TOK_COLON,
    TOK_COMMA,
    TOK_PLUS,
    TOK_MINUS,
    TOK_FULLSTOP,

    TOK_DEC_NUM,
    TOK_HEX_NUM
} token_type;

typedef struct token {
    token_type type;
    char* val;
    size_t line_num, col_num;
} token;

token* new_tok(token_type type, char* val, size_t line_num, size_t col_num);
void print_tok(size_t i, token* tok);

#endif