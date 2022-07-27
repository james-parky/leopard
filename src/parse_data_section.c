#include "include/parser.h"

#include <string.h>

#include "include/error.h"
#include "include/token.h"

/*****************************************************************************
 * parse_data_section: Parses the full .data section of a MIPS program.      *
 *                                                                           *
 * <var_decs> ::= <var_dec> | <var_dec><var_decs>                            *                                                     
 * <data_section> ::= ".data"<var_decs>                                      *
 *****************************************************************************/
void parse_data_section (parser* parser) {
    if (accept(parser, TOK_FULLSTOP)) {
        if (accept(parser, TOK_IDENTIFIER)) {
            if (strcmp(prev_tok(parser, 1)->val, "data")!= 0) 
                throw_unexpected_symbol_error(parser, ERROR_DATA);
            while (parser->current_tok 
                    && (parser->current_tok->type != TOK_FULLSTOP)) {
                if (accept(parser, TOK_IDENTIFIER)) {
                    if (accept(parser, TOK_COLON)) 
                        parse_variable_declaration(parser, 
                                                    prev_tok(parser, 2)->val);
                    else
                        throw_unexpected_symbol_error(parser, ERROR_VAR_TYPE);
                } else throw_unexpected_symbol_error(parser, ERROR_LABEL);
            }
        }
    }
}