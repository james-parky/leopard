#include "include/parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/error.h"
#include "include/instruction.h"
#include "include/lexer.h"
#include "include/operand.h"
#include "include/token.h"
#include "include/value.h"
#include "include/variable.h"

parser* parser_init (lexer* lex) {
    parser* parser = malloc(sizeof(parser));
    if (!parser) return NULL;
    parser->lex = lex;
    parser->instr_buf_index = 0;
    parser->instr_count = 0;
    parser->label_buf_index = 0;
    parser->label_count = 0;
    parser->var_buf_index = 0;
    parser->var_count = 0;
    parser->current_tok = parser->lex->tok_buf[0];
    return parser;
}

token* prev_tok (parser* parser, const int num) {
    return parser->lex->tok_buf[parser->lex->tok_buf_index - num];
}

token* next_tok (parser* parser) {
    parser->lex->tok_buf_index++;
    if(parser->lex->tok_buf_index > parser->lex->tok_count) return NULL;
    return parser->lex->tok_buf[parser->lex->tok_buf_index];
}

void str_toupper (char* str) {
    size_t i = 0;
    while (str[i]) {str[i] = toupper(str[i]); i++;}
}

variable* prev_var (parser* parser, const size_t num) {
    return parser->var_buf[parser->var_buf_index - num];
}

instruction* prev_instr (parser* parser, const size_t num){
    return parser->instr_buf[parser->instr_buf_index - num];
}

int accept (parser* parser, const token_type t) {
    if (!parser->current_tok) return 0;
    if (parser->current_tok->type == t) {
        parser->current_tok = next_tok(parser);
        return 1;
    }
    return 0;
}

int expect (parser* parser, const token_type t, int error_code) {
    if (accept(parser, t)) return 1;
    throw_unexpected_symbol_error(parser, error_code);
    exit(1);
}

void parser_add_var (parser* parser, variable_type type, char* var_name) {
    variable* var = new_variable(type, var_name);
    parser->var_buf[parser->var_buf_index] = var;
    parser->var_buf_index++;
    parser->var_count++;
}

void parser_add_instruction (parser* parser, char* opcode) {
    instruction* instr = new_instruction(opcode);
    parser->instr_buf[parser->instr_buf_index] =  instr;
    parser->instr_buf_index++;
}

void parser_add_label (parser* parser, char* label_name) {
    label* lab = new_label(parser->instr_buf_index, label_name);
    parser->label_buf[parser->label_buf_index] = lab;
    parser->label_buf_index++;
}

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

void parse_dec_number_data_value (parser* parser) {
    if (accept(parser, TOK_COLON)) {
        if (accept(parser, TOK_DEC_NUM)) {
            for (size_t i = 0; i < (size_t)atoi(prev_tok(parser, 1)->val); i++)
                variable_add_value(parser->current_var, VAL_DEC_NUM, 
                                    prev_tok(parser, 3)->val);
        } else if (accept(parser, TOK_HEX_NUM)) {
            for (size_t i = 0; 
                    i < (size_t)strtol(prev_tok(parser, 1)->val, NULL, 16);
                    i++)
                variable_add_value(parser->current_var, VAL_DEC_NUM, 
                                    prev_tok(parser, 3)->val);
        }
    } else
        variable_add_value(parser->current_var, VAL_DEC_NUM, 
                            prev_tok(parser, 1)->val);
}

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

void parse_data_list (parser* parser) {
    do {
        parse_data_value(parser);
    } while (accept(parser, TOK_COMMA));
}

void parse_variable_declaration (parser* parser, char* var_name) {
    if(accept(parser, TOK_FULLSTOP)) {
        expect(parser, TOK_IDENTIFIER, ERROR_VAR_DEC);
        parser->current_tok = prev_tok(parser, 1);
        parser->lex->tok_buf_index--;
        char* data_type = parser->current_tok->val;
        str_toupper(data_type);

        if ((strcmp(data_type, "ASCII") == 0) 
            || (strcmp(data_type, "ASCIIZ") == 0)) {

            printf("found ascii or asciiz\n");
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
            printf("val val: %s\n", parser->current_var->value_buf[0]->val);

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

void parse_register_name (parser* parser) {
    if (accept(parser, TOK_DOLLAR_SIGN)) {
        if ((parser->current_tok->type == TOK_DEC_NUM)
            || (parser->current_tok->type == TOK_IDENTIFIER)) 
            parser->current_tok = next_tok(parser);
        else throw_unexpected_symbol_error(parser, ERROR_REG);
    } else throw_unexpected_symbol_error(parser, ERROR_REG);
}

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

    else if (accept(parser, TOK_DOLLAR_SIGN)) {
        
        if ((parser->current_tok->type == TOK_DEC_NUM) 
            || (parser->current_tok->type == TOK_IDENTIFIER)) {
            const size_t op_len = strlen(parser->current_tok->val);
            char* reg = malloc(sizeof(char) * op_len + 1);
            strcpy(reg, parser->current_tok->val);
            reg[op_len] = '\0';
            instruction_add_operand(parser->current_instr, OP_REG, reg);
            parser->current_tok = next_tok(parser);
            free(reg);
        } else throw_unexpected_symbol_error(parser, ERROR_REG);
    }
    
}   

void parse_operand_list (parser* parser) {
    do{
        parse_operand(parser);
    } while(accept(parser, TOK_COMMA));
    
}

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

void parse_text_section (parser* parser) {
    if (accept(parser, TOK_FULLSTOP)) {
        if (accept(parser, TOK_IDENTIFIER)) {
            if (strcmp(prev_tok(parser, 1)->val, "text") != 0)
                throw_unexpected_symbol_error(parser, ERROR_TEXT);
            if (accept(parser, TOK_FULLSTOP)) {
                if (accept(parser, TOK_IDENTIFIER)) {
                    if (strcmp(prev_tok(parser, 1)->val, "globl") != 0)
                        throw_unexpected_symbol_error(parser, ERROR_GLOBL);
                    if (accept(parser, TOK_IDENTIFIER) 
                        && parser->current_tok->type!=TOK_COLON) {
                        parser->entry_point = prev_tok(parser, 1)->val;
                    } else throw_unexpected_symbol_error(parser, ERROR_ENTRY);
                } else throw_unexpected_symbol_error(parser, ERROR_LABEL_OR_INSTR);
            }

            while (parser->current_tok->type!=TOK_EOF 
                    && (parser->current_tok->type != TOK_FULLSTOP)) {
                if (accept(parser, TOK_IDENTIFIER)) {
                    if (accept(parser, TOK_COLON)) {
                        parser_add_label(parser, prev_tok(parser, 2)->val);
                    } else {
                        parser->current_tok = prev_tok(parser, 1);
                        parser->lex->tok_buf_index--;
                        parse_instruction(parser);
                    }
                } else throw_unexpected_symbol_error(parser, ERROR_LABEL_OR_INSTR);
            }
        } else throw_unexpected_symbol_error(parser, ERROR_TEXT);
    } else throw_unexpected_symbol_error(parser, ERROR_TEXT);
}

void parser_parse_program (parser* parser) {
    if (accept(parser, TOK_FULLSTOP)) {
        printf("here\n");
        if (strcmp(parser->current_tok->val, "data") == 0) {
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_data_section(parser);
            parse_text_section(parser);
        } else if (strcmp(parser->current_tok->val, "text") == 0) {
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_text_section(parser);
            parse_data_section(parser);
        } else throw_unexpected_symbol_error(parser, ERROR_TEXT_OR_DATA);
        if (parser->lex->tok_buf_index>parser->lex->tok_count)
            throw_unexpected_symbol_error(parser, ERROR_EOF);
    } else throw_unexpected_symbol_error(parser, ERROR_TEXT_OR_DATA);
}

void parser_reset_buf_indexes (parser* parser) {
    parser->var_count = parser->var_buf_index;
    parser->label_count = parser->label_buf_index;
    parser->instr_count = parser->instr_buf_index;
    parser->instr_buf_index = 0;
    parser->label_buf_index = 0;
    parser->var_buf_index = 0;
}

void free_labels (parser* parser) {
    for (size_t i = 0;i<parser->label_count;i++) 
        free(parser->label_buf[i]);
}

void free_instrs (parser* parser) {
    for (size_t i = 0; i < parser->instr_count; i++) {
        free(parser->instr_buf[i]->operands[0]);
        free(parser->instr_buf[i]->operands[1]);
        free(parser->instr_buf[i]->operands[2]);
        free(parser->instr_buf[i]->opcode);
        free(parser->instr_buf[i]);
    }
}

void free_variables (parser* parser) {
    for (size_t i=0;i<parser->var_count;i++) {
        for (size_t j=0;j<parser->var_buf[i]->value_count;j++)
            free(parser->var_buf[i]->value_buf[j]);
        free(parser->var_buf[i]);
    }
}

void parser_free_all (parser* parser) {
    free_labels(parser);
    free_instrs(parser);
    free_variables(parser);
}