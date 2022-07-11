#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>

#include "token.h"

#define LEX_BUF_MAX 1000

typedef struct {
    char* file_name;
    FILE* file;
    token* tok_buf[LEX_BUF_MAX];
    size_t tok_buf_index;
    size_t tok_count;
} lexer;

/****************************************************************************
 * lexer_init: Creates a new lexer struct, initialise the struct            *
 *             variables and returns a pointer to said lexer.               *
 ****************************************************************************/
lexer* lexer_init(FILE* in_file, char* in_file_name);

/****************************************************************************
 * lexer_lex: Create tokens for each line in the given lexer struct's input *
 *            source file. Store all lexed tokens in the given lexer's      *
 *            token buffer. Remove comment tokens from the buffer if the    *
 *            value of remove_comments is not zero.                         *
 ****************************************************************************/
void lexer_lex(lexer* lex, int remove_comments);

/****************************************************************************
 * lexer_tok_dump: Print all tokens in the given lexer's token buffer to    *
 *                 stdout.                                                  *
 ****************************************************************************/
void lexer_tok_dump(lexer* lex);

/****************************************************************************
 * lexer_reset_buf_index: Resets the token buffer index and sets the token  *
 *                        count for the given lexer struct.                 *
 ****************************************************************************/
void lexer_reset_buf_index(lexer* lex);

#endif