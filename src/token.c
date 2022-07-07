#include <stdio.h>
#include <stdlib.h>
#include "token.h"

token* new_tok(token_type type, char* val, size_t line_num, size_t col_num){
    token* tok = malloc(sizeof(token));
    tok->type = type;
    tok->val = val;
    tok->line_num = line_num;
    tok->col_num = col_num;
    return tok;
}

void print_tok(token* tok){
    const char* tok_names[13] = {"IDENTIFIER", "STRING", "COMMENT", "L_PAREN", "R_PAREN", "DOLLAR_SIGN", "COLON", "COMMA", "PLUS", "MINUS", "FULLSTOP", "DEC_NUM", "HEX_NUM"};
    printf("[Ln %zu, Col %zu]: <[TYPE]: %s, [VALUE]: \"%s\">\n", tok->line_num, tok->col_num, tok_names[tok->type], tok->val);
}