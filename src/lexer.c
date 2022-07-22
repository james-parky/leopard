#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

lexer* lexer_init (FILE* file, char* file_name) {
    lexer* lex = malloc(sizeof(lexer));
    lex->file_name = file_name;
    lex->file = file;
    lex->tok_buf_index = 0;
    lex->tok_count = 0;
    return lex;
}

void lexer_add_tok (lexer* lex, token_type type, char* val, size_t line_num,
                    size_t col_num) {
    token* tok = new_tok(type, val, line_num, col_num);
    lex->tok_buf[lex->tok_buf_index] = tok;
    lex->tok_buf_index++;
    lex->tok_count++;
}

int is_empty_line (char* buf) {
    while(*buf) if(!isspace(*buf++)) return 0;
    return 1;
}

size_t get_str_len (char* buf) {
    size_t i = 0;
    while(buf[i] && buf[i] != '"') i++;
    return i;
}

char* get_comment (const char* buf) {
    size_t i = 0;
    while(buf[i] && buf[i] != '\n') i++;
    char* comment = malloc(sizeof(char) * i + 1);
    strncpy(comment, buf, i);
    comment[i + 1] = '\0';
    return comment;
}

size_t get_identifier_len (char* buf) {
    size_t i = 0;
    while(isalnum(buf[i]) || buf[i] == '_') i++;
    return i;
}

size_t get_num_len (char* buf) {
    size_t i = 0;
    while(buf[i] && (isdigit(buf[i]) || isxdigit(buf[i]))) i++;
    return i;
}

char* substr (const char* buf, size_t len) {
    char* substr = malloc(sizeof(char) * len + 1);
    strncpy(substr, buf, len);
    substr[len + 1] = '\0';
    return substr;
}

void lex_line (lexer* lex, char* buf, size_t line_num) {
    size_t col = 0;
    while(buf[col] && buf[col] != '\n') {
        switch(buf[col]){
            case ' ': break;
            case '\n': break;
            case '\t': break;
            case '-': 
                lexer_add_tok(lex, TOK_MINUS, "-", line_num, col); break; 
            case '+': 
                lexer_add_tok(lex, TOK_PLUS, "+", line_num, col); break;
            case '$': 
                lexer_add_tok(lex, TOK_DOLLAR_SIGN, "$", line_num, col); break;
            case '(': 
                lexer_add_tok(lex, TOK_L_PAREN, "(", line_num, col); break; 
            case ')': 
                lexer_add_tok(lex, TOK_R_PAREN, ")", line_num, col); break; 
            case '.': 
                lexer_add_tok(lex, TOK_FULLSTOP, ".", line_num, col); break; 
            case ':': 
                lexer_add_tok(lex, TOK_COLON, ":", line_num, col); break; 
            case ',': 
                lexer_add_tok(lex, TOK_COMMA, ",", line_num, col); break; 
            case '#': 
                lexer_add_tok(lex, TOK_COMMENT, get_comment(buf + col),
                                line_num, col); 
                return;
            case '"': {
                    size_t str_len = get_str_len(buf + col + 1);
                    char* str = substr(buf + col + 1, str_len);
                    lexer_add_tok(lex, TOK_STRING, str, line_num, col);
                    col += str_len + 1;
                    free(str);
                }
                break;
            default:
                if(isalpha(buf[col]) || buf[col] == '_') {
                    size_t identifier_len = get_identifier_len(buf + col);
                    char* identifier = substr(buf + col, identifier_len);
                    lexer_add_tok(lex, TOK_IDENTIFIER, identifier, line_num,
                                    col);
                    col += identifier_len - 1;
                    free(identifier);
                } else if (isdigit(buf[col])) {
                    if(buf[col] == '0'
                        && (buf[col + 1] == 'x' || buf[col + 1] == 'X')) {
                        size_t hex_num_len = get_num_len(buf + col + 2);
                        char* hex_num = substr(buf + col + 2, hex_num_len);
                        lexer_add_tok(lex, TOK_HEX_NUM, hex_num, line_num, 
                                        col);
                        col += hex_num_len + 1;
                        free(hex_num);
                    } else {
                        size_t dec_num_len = get_num_len(buf + col);
                        char* dec_num = malloc(sizeof(char) * dec_num_len + 1);
                        strncpy(dec_num, buf + col, dec_num_len);
                        lexer_add_tok(lex, TOK_DEC_NUM, dec_num, line_num,
                                        col);
                        col += dec_num_len - 1;
                        free(dec_num);
                    }
                } else throw_non_ascii_error(lex, line_num, col, (wchar_t)buf[col]);
        }
        col++;
    }
}

void remove_token (lexer* lex, size_t index) {
    for (; index < lex->tok_count - 1; index++)
        lex->tok_buf[index] = lex->tok_buf[index + 1];
}

void remove_comment_toks (lexer* lex) {
    size_t rem_count = 0;
    for (size_t i = 0; i < lex->tok_count; i++) {
        if (lex->tok_buf[i]->type == TOK_COMMENT) {
            remove_token(lex, i);
            rem_count++;
        }
        lex->tok_count -= rem_count;
        rem_count = 0;
    }
    lexer_reset_buf_index(lex);
}

void lexer_lex (lexer* lex, int remove_comments) {
    size_t line_num = 0;
    char line_buf[LINE_BUF_MAX];
    while (fgets(line_buf, LINE_BUF_MAX, lex->file) != NULL) {
        if (!is_empty_line(line_buf)) lex_line(lex, line_buf, line_num);
        line_num++;
    }
    lexer_add_tok(lex, TOK_EOF, "", line_num, 0);
    lexer_reset_buf_index(lex);
    if (remove_comments) remove_comment_toks(lex);
}

void lexer_tok_dump (lexer* lex) {
    for (size_t i = 0; i < lex->tok_count; i++)
        print_tok(i, lex->tok_buf[i]);
}

void lexer_reset_buf_index (lexer* lex) {
    lex->tok_buf_index = 0;
    lex->tok_buf[lex->tok_count] = NULL;
}
void lexer_free (lexer* lex){
    for (size_t i = 0; i < lex->tok_count; i++) {
        free(lex->tok_buf[i]->val);
        free(lex->tok_buf[i]);
    }
}