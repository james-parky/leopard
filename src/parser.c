#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instruction.h"
#include "lexer.h"
#include "operand.h"
#include "token.h"
#include "value.h"
#include "variable.h"

#define BOLD "\x1B[1m"
#define GREEN "\033[32;1m"
#define RED "\033[31;1m"
#define RESET "\033[0m"

#define ERROR_VAR_TYPE "variable type '.ascii', '.asciiz', '.space', '.word', '.half', '.byte'"

parser* parser_init(lexer* lex) {
    parser* parser = malloc(sizeof(parser));
    if(!parser) return NULL;
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

token* prev_tok(parser* parser, int num) {
    return parser->lex->tok_buf[parser->lex->tok_buf_index - num];
}

token* next_tok(parser* parser) {
    parser->lex->tok_buf_index++;
    if(parser->lex->tok_buf_index > parser->lex->tok_count) return NULL;
    return parser->lex->tok_buf[parser->lex->tok_buf_index];
}

void str_toupper(char* str) {
    size_t i = 0;
    while(str[i]) {str[i] = toupper(str[i]); i++;}
}

void subst(char *s, char from, char to) {
    while (*s == from)
    *s++ = to;
}

void throw(parser* parser, char* expected) {
    token* tok;
    if(parser->lex->tok_buf_index==0) tok = parser->current_tok;
    else tok = prev_tok(parser, 1);
    char* file_name = parser->lex->file_name;
    size_t error_line_num = tok->line_num;

    FILE* file = fopen(parser->lex->file_name, "r");
    char line_buf[LEX_BUF_MAX];
    for(size_t i=0;i<error_line_num+1;i++){
        fgets(line_buf, LEX_BUF_MAX, file);
    }

    int space = tok->col_num;
    int tok_val_len = strlen(tok->val);
    char tilde_underline[tok_val_len];
    tilde_underline[0]='\0';
    if(tok_val_len>1){
        sprintf(tilde_underline, "%0*d", tok_val_len, 0);
        subst(tilde_underline, '0', '~');
    }

    fprintf(stderr, "%s%s:%zu:%zu: %serror:%s%s expected %s\n", 
            BOLD, file_name, tok->line_num, tok->col_num, RED, RESET, BOLD,
            expected);
    fprintf(stderr, "%s%s", RESET, line_buf);
    fprintf(stderr, "%s%*c%s\n", GREEN, space, '^', tilde_underline);
    exit(1);
}



variable* prev_var(parser* parser, int num) {
    return parser->var_buf[parser->var_buf_index - num];
}

instruction* prev_instr(parser* parser, int num){
    return parser->instr_buf[parser->instr_buf_index - num];
}

int accept(parser* parser, token_type t) {
    if(!parser->current_tok) return 0;
    if(parser->current_tok->type == t) {
        parser->current_tok = next_tok(parser);
        return 1;
    }
    return 0;
}

int expect(parser* parser, token_type t, char* expected) {
    if(accept(parser, t)) return 1;
    throw(parser, expected);
    exit(1);
}

void parser_add_var(parser* parser, variable_type type, char* var_name) {
    variable* var = new_variable(type, var_name);
    if(!var) {
        printf("ERROR: Memory for new variable not allocated.\n");
        exit(1);
    }
    parser->var_buf[parser->var_buf_index] = var;
    parser->var_buf_index++;
    parser->var_count++;
}

void parser_add_instruction(parser* parser, char* opcode) {
    instruction* instr = new_instruction(opcode);
    if(!instr) {
        printf("ERROR: Memory for new instruction not allocated.\n");
        exit(1);
    }
    parser->instr_buf[parser->instr_buf_index] = instr;
    parser->instr_buf_index++;
}

void parser_add_label(parser* parser, char* label_name) {
    label* lab = new_label(parser->instr_buf_index, label_name);
    if(!lab) {
        printf("ERROR: Memory for new label not allocated.\n");
        exit(1);
    }
    parser->label_buf[parser->label_buf_index] = lab;
    parser->label_buf_index++;
}

void parse_negative_data_value(parser* parser) {
    expect(parser, TOK_DEC_NUM, "parse_negative_data_value:1");
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

void parse_dec_number_data_value(parser* parser) {
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

void parse_hex_number_data_value(parser* parser) {
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

void parse_data_value(parser* parser) {
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
        throw(parser, "variable declaration");
}

void parse_data_list(parser* parser) {
    do {
        parse_data_value(parser);
    } while(accept(parser, TOK_COMMA));
}

void parse_variable_declaration(parser* parser, char* var_name) {
    if(accept(parser, TOK_FULLSTOP)){
        expect(parser, TOK_IDENTIFIER, "parse_variable_declaration:1");
        parser->current_tok = prev_tok(parser, 1);
        parser->lex->tok_buf_index--;
        char* data_type = parser->current_tok->val;
        str_toupper(data_type);

        if((strcmp(data_type, "ASCII") == 0) 
            || (strcmp(data_type, "ASCIIZ") == 0)) {

            printf("found ascii or asciiz\n");
            parser->current_tok = next_tok(parser);
            expect(parser, TOK_STRING, "parse_variable_declaration:2");

            variable_type type;
            type = (strcmp(data_type, "ASCII") == 0) ? VAR_ASCII : VAR_ASCIIZ;
            parser_add_var(parser, type, var_name);
            parser->current_var = prev_var(parser, 1);

            if(strcmp(data_type, "ASCII") == 0)
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
            if(accept(parser, TOK_DEC_NUM)){
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
                throw(parser, "number");
        }
    } else throw(parser, ERROR_VAR_TYPE);
}

void parse_data_section(parser* parser) {
    printf("parsing data sec\n");
    if(accept(parser, TOK_FULLSTOP)){
        if(accept(parser, TOK_IDENTIFIER)) {
            if(strcmp(prev_tok(parser, 1)->val, "data")!= 0) 
                throw(parser, ".data header");
            while(parser->current_tok 
                    && (parser->current_tok->type != TOK_FULLSTOP)) {
                if(accept(parser, TOK_IDENTIFIER)) {
                    if(accept(parser, TOK_COLON)) 
                        parse_variable_declaration(parser, 
                                                    prev_tok(parser, 2)->val);
                    else
                        throw(parser, ERROR_VAR_TYPE);
                } else throw(parser, "label");
            }
        }
    }
}

void parse_register_name(parser* parser){
    if(accept(parser, TOK_DOLLAR_SIGN)){
        if((parser->current_tok->type == TOK_DEC_NUM)
            || (parser->current_tok->type == TOK_IDENTIFIER)) 
            parser->current_tok = next_tok(parser);
        else throw(parser, "register");
    } else throw(parser, "register");
}

void parse_operand(parser* parser) {
    if (accept(parser, TOK_L_PAREN)) {
        parse_register_name(parser);
        expect(parser, TOK_R_PAREN, "right parenthesis");
        instruction_add_operand(parser->current_instr, OP_REG,
                                prev_tok(parser, 2)->val);
    }

    else if (accept(parser, TOK_MINUS)) {
        expect(parser, TOK_DEC_NUM, "decimal number");
        instruction_add_operand(parser->current_instr, OP_NEG_DEC_NUM,
                                prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_DEC_NUM)) {
        if (accept(parser, TOK_L_PAREN)) {
            parse_register_name(parser);
            expect(parser, TOK_R_PAREN, "right parenthesis");
            char* inx_reg = NULL;
            sprintf(inx_reg, "%s(%s)", prev_tok(parser, 5)->val, 
                    prev_tok(parser, 2)->val);
            instruction_add_operand(parser->current_instr, OP_INX_REG, 
                                    inx_reg);
        } else
            instruction_add_operand(parser->current_instr, OP_DEC_NUM, 
                                    prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_HEX_NUM))
        instruction_add_operand(parser->current_instr, OP_HEX_NUM,
                                prev_tok(parser, 1)->val);

    else if (accept(parser, TOK_IDENTIFIER)) {
        if (accept(parser, TOK_PLUS)) {
            expect(parser, TOK_DEC_NUM, "decimal number");
            if (accept(parser, TOK_L_PAREN)) {
                parse_register_name(parser);
                expect(parser, TOK_R_PAREN, "right parenthesis");
                char* idn_plus_inx_reg = NULL;
                sprintf(idn_plus_inx_reg, "%s+%s(%s)", 
                        prev_tok(parser, 2)->val, prev_tok(parser, 5)->val, 
                        prev_tok(parser, 7)->val);
                instruction_add_operand(parser->current_instr, 
                                        OP_IDN_PLUS_INX_REG, idn_plus_inx_reg);
            } else {
                char* idn_dec_num = NULL;
                sprintf(idn_dec_num, "%s+%s", prev_tok(parser, 3)->val, 
                        prev_tok(parser, 1)->val);
                instruction_add_operand(parser->current_instr, 
                                        OP_IDN_DEC_NUM, idn_dec_num);
            }
        } else if (accept(parser, TOK_L_PAREN)) {
            parse_register_name(parser);
            expect(parser, TOK_R_PAREN, "right parenthesis");
            char* idn_inx_reg = NULL;
            sprintf(idn_inx_reg, "%s(%s)", prev_tok(parser, 5)->val, 
                    prev_tok(parser, 2)->val);
            instruction_add_operand(parser->current_instr, OP_IDN_INX_REG,
                                    idn_inx_reg);
        } else
            instruction_add_operand(parser->current_instr, OP_IDN,
                                    prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_DOLLAR_SIGN)) {
        
        if ((parser->current_tok->type == TOK_DEC_NUM) 
            || (parser->current_tok->type == TOK_IDENTIFIER)) {
            instruction_add_operand(parser->current_instr, OP_REG,
                                    parser->current_tok->val);
            parser->current_tok = next_tok(parser);
        } else throw(parser, "register");
    }
    
}   


void parse_operand_list(parser* parser) {
    do{
        parse_operand(parser);
    } while(accept(parser, TOK_COMMA));
    
}

void parse_instruction(parser* parser) {
    char* opcode = NULL;
    if(accept(parser, TOK_IDENTIFIER)) {
        if(accept(parser, TOK_FULLSTOP)) {
            if(accept(parser, TOK_IDENTIFIER)) 
                sprintf(opcode, "%s.%s", prev_tok(parser, 3)->val,
                        prev_tok(parser, 1)->val);
            else throw(parser, "instruction");
        } else opcode = prev_tok(parser, 1)->val;

        str_toupper(opcode);
        parser_add_instruction(parser, opcode);
        parser->current_instr = prev_instr(parser, 1);

        if((strcmp(opcode, "SYSCALL")!=0) && parser->current_tok)
            parse_operand_list(parser);
    } else throw(parser, "instruction");
}

void parse_text_section(parser* parser) {
    if(accept(parser, TOK_FULLSTOP)) {
        if(accept(parser, TOK_IDENTIFIER)) {
            if(strcmp(prev_tok(parser, 1)->val, "text") != 0)
                throw(parser, ".text header");
            if(accept(parser, TOK_FULLSTOP)) {
                if(accept(parser, TOK_IDENTIFIER)) {
                    if(strcmp(prev_tok(parser, 1)->val, "globl") != 0)
                        throw(parser, ".globl header");
                    if(accept(parser, TOK_IDENTIFIER) 
                        && parser->current_tok->type!=TOK_COLON) {
                        parser->entry_point = prev_tok(parser, 1)->val;
                    } else throw(parser, "entry point definition");
                } else throw(parser, "label or instruction");
            }

            while( parser->current_tok 
                    && (parser->current_tok->type != TOK_FULLSTOP)) {
                if(accept(parser, TOK_IDENTIFIER)){
                    if(accept(parser, TOK_COLON)){
                        parser_add_label(parser, prev_tok(parser, 2)->val);
                    } else{
                        parser->current_tok = prev_tok(parser, 1);
                        parser->lex->tok_buf_index--;
                        parse_instruction(parser);
                    }
                } else throw(parser, "label or instruction");
            }
        } else throw(parser, ".text header");
    } else throw(parser, ".text header");
}

void parser_parse_program(parser* parser) {
    if(accept(parser, TOK_FULLSTOP)){
        if(strcmp(parser->current_tok->val, "data") == 0) {
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_data_section(parser);
            parse_text_section(parser);
        } else if(strcmp(parser->current_tok->val, "text") == 0) {
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_text_section(parser);
            parse_data_section(parser);
        } else throw(parser, ".text or .data header");
        if(parser->lex->tok_buf_index>parser->lex->tok_count)
            throw(parser, "end of file");
    } else throw(parser, ".text or .data header");
}

void parser_reset_buf_indexes(parser* parser) {
    printf("%zu\n", parser->var_buf_index);
    parser->var_count = parser->var_buf_index;
    parser->label_count = parser->label_buf_index;
    parser->instr_count = parser->instr_buf_index;
    parser->instr_buf_index = 0;
    parser->label_buf_index = 0;
    parser->var_buf_index = 0;
}