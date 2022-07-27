#include "include/parser.h"

#include "include/token.h"

/*****************************************************************************
 * parse_data_list: Continuously parses data values as long as commans are   *
 *                  also parsed in-between.                                  *
 *                                                                           *
 * <data_list> ::= <data_value> | <data_value>","<data_list>                 *
 *****************************************************************************/
void parse_data_list (parser* parser) {
    do {
        parse_data_value(parser);
    } while (accept(parser, TOK_COMMA));
}