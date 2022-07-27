#include "include/parser.h"

#include <stdlib.h>
#include <string.h>

#include "include/error.h"
#include "include/token.h"

/*****************************************************************************
 * parse_instruction: Parses a full instruction call within MIPS - which     *
 *                    consists of an opcode and a set of operands. Or just   *
 *                    an opcode in the case of a syscall.                    *
 *                                                                           *
 * <instruction> ::= <opcode> | <opcode><operand_list>                       * 
 *****************************************************************************/
void parse_instruction (parser* parser) {
    if (accept(parser, TOK_IDENTIFIER)) {
        if (accept(parser, TOK_FULLSTOP)) {
            if (accept(parser, TOK_IDENTIFIER)) {
                size_t len = strlen(prev_tok(parser, 3)->val)
                            + strlen(prev_tok(parser, 1)->val) + 1;
                char* opcode = malloc(sizeof(char) * len + 1);
                sprintf(opcode, "%s.%s", prev_tok(parser, 3)->val,
                        prev_tok(parser, 1)->val);
                opcode[len] = '\0';
                str_toupper(opcode);
                parser_add_instruction(parser, opcode);
                free(opcode);
            }
            else throw_unexpected_symbol_error(parser, ERROR_INSTR);
        } else {
            size_t len = strlen(prev_tok(parser, 1)->val);
            char* opcode = malloc(sizeof(char) * len + 1);
            strncpy(opcode, prev_tok(parser, 1)->val, len);
            opcode[len] = '\0';
            str_toupper(opcode);
            parser_add_instruction(parser, opcode);
            free(opcode);
        } 
        parser->current_instr = prev_instr(parser, 1);

        if (parser->current_tok && (strcmp(parser->current_instr->opcode,
                                            "SYSCALL") != 0))
            parse_operand_list(parser);
    } else throw_unexpected_symbol_error(parser, ERROR_INSTR);
}