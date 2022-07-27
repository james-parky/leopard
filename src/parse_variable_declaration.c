#include "include/parser.h"

#include <string.h>

#include "include/error.h"
#include "include/token.h"
#include "include/value.h"
#include "include/variable.h"

/*****************************************************************************
 * parse_variable_declaration: Parses a MIPS data type followed by a value   *
 *                             or data_list appropriate to that data type.   *
 *                                                                           *
 * <data_type> ::= "ascii" | "asciiz" | "word" | "half" | "byte" | "space"   *
 * <var_dec> ::= "."<data_type><data_list>                                   *
 *****************************************************************************/
void parse_variable_declaration (parser* parser, char* var_name) {
    if(accept(parser, TOK_FULLSTOP)) {
        expect(parser, TOK_IDENTIFIER, ERROR_VAR_DEC);
        parser->current_tok = prev_tok(parser, 1);
        parser->lex->tok_buf_index--;
        char* data_type = parser->current_tok->val;
        str_toupper(data_type);

        if ((strcmp(data_type, "ASCII") == 0) 
            || (strcmp(data_type, "ASCIIZ") == 0)) {

            parser->current_tok = next_tok(parser);
            expect(parser, TOK_STRING, ERROR_VAR_DEC);
            variable_type type;
            type = (strcmp(data_type, "ASCII") == 0) ? VAR_ASCII : VAR_ASCIIZ;
            parser_add_var(parser, type, var_name);
            parser->current_var = prev_var(parser, 1);

            if (strcmp(data_type, "ASCII") == 0)
                variable_add_value(parser->current_var, VAL_ASCII,
                                    prev_tok(parser, 1)->val);
            else variable_add_value(parser->current_var, VAL_ASCIIZ,
                                    prev_tok(parser, 1)->val);

        } else if ((strcmp(data_type, "WORD") == 0)
                    || (strcmp(data_type, "HALF") == 0) 
                    || (strcmp(data_type, "BYTE") == 0)) {

            parser->current_tok = next_tok(parser);

            variable_type type = (strcmp(data_type, "WORD") == 0)
                                    ? VAR_WORD 
                                    : (strcmp(data_type, "HALF") == 0) 
                                        ? VAR_HALF 
                                        : VAR_BYTE;

            parser_add_var(parser, type, var_name);
            parser->current_var = prev_var(parser, 1);

            parse_data_list(parser);

        } else if (strcmp(data_type, "SPACE") == 0) {
            parser->current_tok = next_tok(parser);
            if (accept(parser, TOK_DEC_NUM)){
                parser_add_var(parser, VAR_SPACE, var_name);
                parser->current_var = prev_var(parser, 1);
                variable_add_value(parser->current_var, VAL_DEC_NUM, 
                                    prev_tok(parser, 1)->val);

            } else if (accept(parser, TOK_HEX_NUM)){
                parser_add_var(parser, VAR_SPACE, var_name);
                parser->current_var = prev_var(parser, 1);
                variable_add_value(parser->current_var, VAL_HEX_NUM, 
                                    prev_tok(parser, 1)->val);

            } else 
                throw_unexpected_symbol_error(parser, ERROR_NUM);
        }
    } else throw_unexpected_symbol_error(parser, ERROR_VAR_TYPE);
}