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
    TOK_HEX_NUM,
    TOK_EOF
} token_type;

typedef struct token {
    token_type type;
    size_t line_num, col_num;
    char* val;
} token;

/*****************************************************************************
 * new_tok: Creates a new token struct using the given arguments and returns *
 *          a pointer to said token.                                         * 
 *****************************************************************************/
token* new_tok(token_type type, char* val, size_t line_num, size_t col_num);

/*****************************************************************************
 * print_tok: Prints the given token to stdout.                              * 
 *****************************************************************************/
void print_tok(size_t i, token* tok);

#endif