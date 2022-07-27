#include "include/parser.h"

#include <stdlib.h>

#include "include/error.h"
#include "include/token.h"

/*****************************************************************************
 * parse_hex_number_data_value: Parses either a single base 16 number or     * 
 *                              a MIPS style, colon-multiple-initialization  *
 *                              list pattern declaration.                    *
 *                                                                           *
 * <hex_value> ::= <hex_num>                                                 *
 *              |  <hex_num>":"<dec_num>                                     *
 *              |  <hex_num>":"<hex_num>                                     *
 *****************************************************************************/
void parse_hex_number_data_value (parser* parser) {
    if (accept(parser, TOK_COLON)) {
        if (accept(parser, TOK_DEC_NUM)) {
            for (size_t i = 0; i < (size_t)atoi(prev_tok(parser, 1)->val); i++)
                variable_add_value(parser->current_var, VAL_HEX_NUM,
                                    prev_tok(parser, 3)->val);
        } else if (accept(parser, TOK_HEX_NUM)) {
            for (size_t i = 0;
                    i < (size_t)strtol(prev_tok(parser, 1)->val, NULL, 16);
                    i++)
                variable_add_value(parser->current_var, VAL_HEX_NUM,
                                    prev_tok(parser, 3)->val);
        }
    } else
        variable_add_value(parser->current_var, VAL_HEX_NUM, 
                            prev_tok(parser, 1)->val);
}