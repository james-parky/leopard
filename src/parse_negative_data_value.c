#include "include/parser.h"

#include <stdlib.h>

#include "include/error.h"
#include "include/token.h"

/*****************************************************************************
 * parse_negative_data_value: Parses either a single negative number or MIPS *
 *                            style, colon-multiple-initialization list      *
 *                            pattern declaration.                           *
 *                                                                           *
 * <neg_value> ::= "-"<dec_num>                                              *
 *              |  "-"<dec_num>":"<dec_num>                                  *
 *              |  "-"<dec_num>":"<hex_num>                                  *
 *****************************************************************************/
void parse_negative_data_value (parser* parser) {
    expect(parser, TOK_DEC_NUM, ERROR_NEG_NUM);
    if (accept(parser, TOK_COLON)) {
        if (accept(parser, TOK_DEC_NUM)) {
            for (size_t i = 0; i < (size_t)atoi(prev_tok(parser, 1)->val); i++)
                variable_add_value(parser->current_var, VAL_NEG_DEC_NUM, 
                                    prev_tok(parser, 3)->val);
        } else if (accept(parser, TOK_HEX_NUM)) {
            for (size_t i = 0;
                    i < (size_t)strtol(prev_tok(parser, 1)->val, NULL, 16);
                    i++)
                variable_add_value(parser->current_var, VAL_NEG_DEC_NUM, 
                                    prev_tok(parser, 3)->val);
        }
    } else
        variable_add_value(parser->current_var, VAL_NEG_DEC_NUM,
                            prev_tok(parser, 1)->val);
}