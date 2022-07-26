#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/instruction.h"
#include "include/lexer.h"
#include "include/parser.h"

#define BOLD "\x1B[1m"
#define GREEN "\033[32;1m"
#define RED "\033[31;1m"
#define RESET "\033[0m"

void subst (char *s, const char from, const char to) {
    while (*s == from) 
    *s++ = to;
}

void print_error_header (const char* file_name, const size_t line_num,
                            const size_t col_num) {
    fprintf(stderr, "%s%s:%zu:%zu: %serror:%s%s", BOLD, file_name, line_num,
            col_num, RED, RESET, BOLD);
}

void print_underline (token* tok) {
    const int space = tok->col_num;
    const int tok_val_len = strlen(tok->val);
    char tilde_underline[tok_val_len];
    tilde_underline[0] = '\0';
    if (tok_val_len > 1) {
        sprintf(tilde_underline, "%0*d", tok_val_len - 1, 0);
        subst(tilde_underline, '0', '~');
    }
    fprintf(stderr, "%s%*c%s\n", GREEN, space, '^', tilde_underline);
}

void print_error_line (const size_t error_line_num, const char* file_name) {
    FILE* file = fopen(file_name, "r");
    char line_buf[LINE_BUF_MAX];
    for (size_t i = 0; i < error_line_num + 1; i++) {
        fgets(line_buf, LINE_BUF_MAX, file);
    }
    fprintf(stderr, "%s%s", RESET, line_buf);
    fclose(file);
}

void print_expected(const int error_code) {
    const char* errors[16] = {"negative number", "variable declaration",
                                "number", ".data header", "label", "register",
                                "right parenthesis", "decimal number",
                                "instruction", "entry point definition",
                                "label or instruction", ".text header",
                                ".text or .data header", "end of file",
                                "variable type '.ascii', '.asciiz', '.space', '.word', '.half', '.byte'",
                                ".globl header"};
    fprintf(stderr, " expected %s\n", errors[error_code]);
}

void throw_unexpected_symbol_error (parser* parser, const int error_code) {
    token* tok = parser->lex->tok_buf_index == 0 ? parser->current_tok
                                                 : prev_tok(parser, 1);
    print_error_header(parser->lex->file_name, tok->line_num, tok->col_num);
    print_expected(error_code);
    print_error_line(tok->line_num, parser->lex->file_name);
    print_underline(tok);
    exit(1);
}

void throw_non_ascii_error (lexer* lex, size_t line_num, size_t col_num,
                            wchar_t wc) {
    print_error_header(lex->file_name, line_num, col_num);
    fprintf(stderr, " non-ASCII characters are not allowed\n");
    print_error_line(line_num, lex->file_name);
    fprintf(stderr, "%s%*c%s\n", GREEN, (int)col_num + 1, '^', RESET);
    exit(1);
}

void throw_invalid_reg_error (parser* parser, token* tok) {
    print_error_header(parser->lex->file_name, tok->line_num, tok->col_num);
    fprintf(stderr, " register not recognized\n");
    print_error_line(tok->line_num, parser->lex->file_name);
    print_underline(tok);
    exit(1);
}