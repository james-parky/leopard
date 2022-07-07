#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include "token.h"

#define BUF_MAX 1000

typedef struct {
    FILE* file;
    token* tok_buf[BUF_MAX];
    size_t tok_buf_index;
    size_t tok_count;
} lexer;
 
lexer* lexer_init(FILE* file);
void lexer_lex(lexer* lex);
void lexer_tok_dump(lexer* lex);
void lexer_reset_buf_index(lexer* lex);
#endif