#include "include/token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token* new_tok (token_type type, char* val, size_t line_num, size_t col_num) {
    token* tok = malloc(sizeof(token));
    tok->type = type;
    tok->val = strdup(val);
    tok->line_num = line_num;
    tok->col_num = col_num;
    return tok;
}

void print_tok (size_t i, token* tok) {
    const char* tok_names[13] = {"IDENTIFIER", "STRING", "COMMENT", "L_PAREN",
                                    "R_PAREN", "DOLLAR_SIGN", "COLON", "COMMA",
                                    "PLUS", "MINUS", "FULLSTOP", "DEC_NUM",
                                    "HEX_NUM"};
    printf("[%zu]: [Ln %zu, Col %zu]: <[TYPE]: %s, [VALUE]: \"%s\">\n", i,
            tok->line_num, tok->col_num, tok_names[tok->type], tok->val);
}