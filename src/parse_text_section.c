#include "include/parser.h"

#include <string.h>

#include "include/token.h"
#include "include/error.h"


/*****************************************************************************
 * parse_text_section: Parses the entire text section of a MIPS program.     *
 *                     This consists of an optional entry point declaration  *
 *                     followed by some number of instructions and labels.   *
 *                                                                           *
 * <label> ::= <string>":"                                                   *
 * <text_statement> ::= <label> | <instruction>                              *
 * <text_body> ::= <text_statement> | <text_statement><text_body>            *
 * <text_section> ::= ".text"".globl"<string><text_body>                     *
 *                 |  ".text"<text_body>                                     *
 *****************************************************************************/
void parse_text_section (parser* parser) {
    if (accept(parser, TOK_FULLSTOP)) {
        if (accept(parser, TOK_IDENTIFIER)) {
            if (strcmp(prev_tok(parser, 1)->val, "text") != 0)
                throw_unexpected_symbol_error(parser, ERROR_TEXT);
            if (accept(parser, TOK_FULLSTOP)) {
                if (accept(parser, TOK_IDENTIFIER)) {
                    if (strcmp(prev_tok(parser, 1)->val, "globl") != 0)
                        throw_unexpected_symbol_error(parser, ERROR_GLOBL);
                    if (accept(parser, TOK_IDENTIFIER) 
                        && parser->current_tok->type!=TOK_COLON) {
                        parser->entry_point = prev_tok(parser, 1)->val;
                    } else throw_unexpected_symbol_error(parser, ERROR_ENTRY);
                } else throw_unexpected_symbol_error(parser, ERROR_LABEL_OR_INSTR);
            }

            while (parser->current_tok->type!=TOK_EOF 
                    && (parser->current_tok->type != TOK_FULLSTOP)) {
                if (accept(parser, TOK_IDENTIFIER)) {
                    if (accept(parser, TOK_COLON)) {
                        parser_add_label(parser, prev_tok(parser, 2)->val);
                    } else {
                        parser->current_tok = prev_tok(parser, 1);
                        parser->lex->tok_buf_index--;
                        parse_instruction(parser);
                    }
                } else throw_unexpected_symbol_error(parser, ERROR_LABEL_OR_INSTR);
            }
        } else throw_unexpected_symbol_error(parser, ERROR_TEXT);
    } else throw_unexpected_symbol_error(parser, ERROR_TEXT);
}