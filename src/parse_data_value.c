#include "include/parser.h"

#include "include/error.h"

/*****************************************************************************
 * parse_data_value: Parse either a positive / negative number of base 10 or *
 *                   or base 16, or an identifier. Throws an unexpected      *
 *                   symbol is neither is found.                             *
 *                                                                           *
 * <data_value> ::= <neg_value> | <dec_value> | <hex_value>                  *
 *****************************************************************************/
void parse_data_value (parser* parser) {
    if (accept(parser, TOK_MINUS))
        parse_negative_data_value(parser);
    else if (accept(parser, TOK_DEC_NUM))
        parse_dec_number_data_value(parser);
    else if (accept(parser, TOK_HEX_NUM))
        parse_hex_number_data_value(parser);
    else if (accept(parser, TOK_IDENTIFIER))
        variable_add_value(parser->current_var, VAL_IDENTIFIER, 
                            prev_tok(parser, 1)->val);
    else 
        throw_unexpected_symbol_error(parser, ERROR_VAR_DEC);
}