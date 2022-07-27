#include "include/parser.h"

#include "include/token.h"

/*****************************************************************************
 * parse_operand_list: Continuously parses operands as long as there are     *
 *                     commas in-between.                                    *
 *                                                                           *
 * <operand_list> ::= <operand> | <operand>","<operand_list>                 *
 *****************************************************************************/
void parse_operand_list (parser* parser) {
    do{
        parse_operand(parser);
    } while(accept(parser, TOK_COMMA));
    
}