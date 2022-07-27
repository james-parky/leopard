#include "include/parser.h"

#include <string.h>

#include "include/error.h"
#include "include/token.h"

/*****************************************************************************
 * is_valid_reg: Returns whether the value for a given token is a valid      *
 *               register within the MIPS language.                          *
 *****************************************************************************/
int is_valid_reg(token* tok) {
    const char* name_regs[32] = {"zero", "at", "v0", "v1", "a0", "a1",
                                    "a2", "a3", "t0", "t1", "t2", "t3",
                                    "t4", "t5", "t6", "t7", "s0", "s1",
                                    "s2", "s3", "s4", "s5", "s6", "s7",
                                    "t8", "t9", "k0", "k1", "gp", "sp",
                                    "fp", "ra"};
    const char* num_regs[32] = {"0", "1", "2", "3", "4", "5", "6",
                                "7", "8", "9", "10", "11", "12",
                                "13", "14", "15", "16", "17",
                                "18", "19", "20", "21", "22",
                                "23", "24", "25", "26", "27",
                                "28", "29", "30", "31"};

    for (short i = 0; i < 32; i++) {
        if(strcmp(tok->val, num_regs[i]) == 0) return 1;
        if(strcmp(tok->val, name_regs[i]) == 0) return 1;
    }
    return 0;
}

/*****************************************************************************
 * parse_register_name: Parses a register within MIPS.                       *
 *                                                                           *
 * <num_reg> ::= "0" | "1" | ... | "32"                                      *
 * <name_reg> ::= "zero" | "at" | "v0" | "v1" | "a0" | "a1" | "a2" | "a3"    *
 *             |  "t0" | "t1" | "t2" | "t3" | "t4" | "t5" | "t6" | "t7"      *
 *             |  "s0" | "s1" | "s2" | "s3" | "s4" | "s5" | "s6" | "s7"      *
 *             |  "t8" | "t9" | "k0" | "k1" | "gp" | "sp" | "fp" | "ra"      *
 * <register> ::= "$"<num_reg> | "$"<name_reg>                               *
 *****************************************************************************/
void parse_register_name (parser* parser) {
    if (accept(parser, TOK_DOLLAR_SIGN)) {
        if ((parser->current_tok->type == TOK_DEC_NUM)
            || (parser->current_tok->type == TOK_IDENTIFIER)) {
                if (!is_valid_reg(parser->current_tok)) 
                    throw_invalid_reg_error(parser, parser->current_tok);
                else parser->current_tok = next_tok(parser);
            }
        else throw_unexpected_symbol_error(parser, ERROR_REG);
    } else throw_unexpected_symbol_error(parser, ERROR_REG);
}