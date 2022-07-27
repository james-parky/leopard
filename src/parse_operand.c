#include "include/parser.h"

#include "include/token.h"
#include "include/error.h"
#include "include/operand.h"
#include "include/error.h"
#include "include/instruction.h"
#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * parse_operand: Parses a valid operand within MIPS - which can be either   *
 *                a register, a register with an offsetting, value, a label  *
 *                or a variable name.                                        *
 *                                                                           *
 * <offset_register> ::= "("<register>")"                                    *
 *                    |  <dec_num>"("<register>")"                           *
 *                    |  <hex_num>"("<register>")"                           *
 *                    |  <string>"+""("<register>")"                         *
 *                    |  <string>"("<register>")"                            *
 * <operand> ::= <register> | <offset_register> | <data_value>               * 
 *****************************************************************************/
void parse_operand (parser* parser) {
    if (accept(parser, TOK_L_PAREN)) {
        parse_register_name(parser);
        expect(parser, TOK_R_PAREN, ERROR_RIGHT_PAREN);
        instruction_add_operand(parser->current_instr, OP_REG,
                                prev_tok(parser, 2)->val);
    }

    else if (accept(parser, TOK_MINUS)) {
        expect(parser, TOK_DEC_NUM, ERROR_DEC_NUM);
        instruction_add_operand(parser->current_instr, OP_NEG_DEC_NUM,
                                prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_DEC_NUM)) {
        if (accept(parser, TOK_L_PAREN)) {
            parse_register_name(parser);
            expect(parser, TOK_R_PAREN, ERROR_RIGHT_PAREN);
            const size_t op_len = strlen(prev_tok(parser, 5)->val)
                                + strlen(prev_tok(parser, 2)->val);
            char* inx_reg = malloc(sizeof(char) * op_len + 1);
            sprintf(inx_reg, "%s%s", prev_tok(parser, 5)->val,
                    prev_tok(parser, 2)->val);
            inx_reg[op_len] = '\0';
            instruction_add_operand(parser->current_instr, OP_INX_REG,
                                    inx_reg);
            free(inx_reg);
        } else {
            instruction_add_operand(parser->current_instr, OP_DEC_NUM,
                                    prev_tok(parser, 1)->val);
        }
    }

    else if (accept(parser, TOK_HEX_NUM)) {
        const size_t op_len = strlen(prev_tok(parser, 1)->val);
        char* hex_num = malloc(sizeof(char) * op_len + 1);
        strncpy(hex_num, prev_tok(parser, 1)->val, op_len);
        hex_num[op_len] = '\0';
        instruction_add_operand(parser->current_instr, OP_HEX_NUM,
                                hex_num);
        free(hex_num);
    }

    else if (accept(parser, TOK_IDENTIFIER)) {
        if (accept(parser, TOK_PLUS)) {
            expect(parser, TOK_DEC_NUM, ERROR_DEC_NUM);
            if (accept(parser, TOK_L_PAREN)) {
                parse_register_name(parser);
                expect(parser, TOK_R_PAREN, ERROR_RIGHT_PAREN);
                const size_t op_len = strlen(prev_tok(parser, 2)->val)
                                    + strlen(prev_tok(parser, 5)->val)
                                    + strlen(prev_tok(parser, 7)->val);
                char* idn_plus_inx_reg = malloc(sizeof(char) * op_len + 1);
                sprintf(idn_plus_inx_reg, "%s+%s%s", prev_tok(parser, 2)->val,
                        prev_tok(parser, 5)->val, prev_tok(parser, 7)->val);
                idn_plus_inx_reg[op_len] = '\0';
                instruction_add_operand(parser->current_instr,
                                        OP_IDN_PLUS_INX_REG, idn_plus_inx_reg);
                free(idn_plus_inx_reg);
            } else {
                const size_t op_len = strlen(prev_tok(parser, 3)->val)
                                    + strlen(prev_tok(parser, 1)->val);
                char* idn_dec_num = malloc(sizeof(char) * op_len + 1);
                sprintf(idn_dec_num, "%s+%s", prev_tok(parser, 3)->val, 
                        prev_tok(parser, 1)->val);
                idn_dec_num[op_len] = '\0';
                instruction_add_operand(parser->current_instr, 
                                        OP_IDN_DEC_NUM, idn_dec_num);
                free(idn_dec_num);
            }
        } else if (accept(parser, TOK_L_PAREN)) {
            parse_register_name(parser);
            expect(parser, TOK_R_PAREN, ERROR_RIGHT_PAREN);
            const size_t op_len = strlen(prev_tok(parser, 5)->val)
                                + strlen(prev_tok(parser, 2)->val);
            char* idn_inx_reg = malloc(sizeof(char) * op_len + 1);
            sprintf(idn_inx_reg, "%s%s", prev_tok(parser, 5)->val, 
                    prev_tok(parser, 2)->val);
            idn_inx_reg[op_len] = '\0';
            instruction_add_operand(parser->current_instr, OP_IDN_INX_REG,
                                    idn_inx_reg);
            free(idn_inx_reg);
        } else {
            const size_t op_len = strlen(prev_tok(parser, 1)->val);
            char* idn = malloc(sizeof(char) * op_len + 1);
            strncpy(idn, prev_tok(parser, 1)->val, op_len);
            idn[op_len] = '\0';
            instruction_add_operand(parser->current_instr, OP_DEC_NUM,
                                    idn);
            free(idn);
        }
    }
    else {
        parse_register_name(parser);
        char* reg = strdup(prev_tok(parser, 1)->val);
        instruction_add_operand(parser->current_instr, OP_REG, reg);
        free(reg);
    }
    
}   