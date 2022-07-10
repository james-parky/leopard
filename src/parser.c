#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "token.h"
#include "value.h"
#include "variable.h"
#include "operand.h"
#include "instruction.h"

parser* parser_init(lexer* lex){
    parser* parser = malloc(sizeof(parser));
    if(parser==NULL) printf("not allocated\n");
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

void str_toupper(char* str){
    int i=0;
    while(str[i]){
        str[i]=toupper(str[i]);
        i++;
    }
}

void throw_unexpected_symbol_exception(char* location, token* tok){
    fprintf(stderr, "[%s] Unexpected symbol: \"%s\" at [Ln %zu, Col %zu].\n", location, tok->val, tok->line_num, tok->col_num);
    exit(1);
}

token* prev_tok(parser* parser, int num){
    token* tok = parser->lex->tok_buf[parser->lex->tok_buf_index - num];
    return tok;
}

token* next_tok(parser* parser){
    parser->lex->tok_buf_index++;
    if(parser->lex->tok_buf_index > parser->lex->tok_count) return NULL;
    return parser->lex->tok_buf[parser->lex->tok_buf_index];
}

variable* prev_var(parser* parser, int num){
    variable* var = parser->var_buf[parser->var_buf_index - num];
    return var;
}

int accept(parser* parser, token_type t){
    if(!parser->current_tok) return 0;
    if(parser->current_tok->type == t){
        parser->current_tok = next_tok(parser);
        return 1;
    }
    return 0;
}

int expect(parser* parser, token_type t, char* location){
    if(accept(parser, t)) return 1;
    throw_unexpected_symbol_exception(location, parser->current_tok);
}

void parser_add_var(parser* parser, variable_type type, char* var_name){
    // printf("len:%zu, val[0]: %s\n", len, values_buf[2]->val);
    // values_buf[len+1]=NULL;
    // variable* var = new_variable(type, var_name, values_buf, len );
    // parser->var_buf[parser->var_buf_index] = var;
    // parser->var_buf_index++;
    // parser->var_count++;
    variable* var = new_variable(type, var_name);
    if(var==NULL) printf("null dec\n");
    parser->var_buf[parser->var_buf_index] = var;
    parser->var_buf_index++;
    //parser->var_count++;
}

void parser_add_instruction(parser* parser, char* opcode, operand* operand_buf[], size_t len){
    instruction* instr = new_instruction(opcode, operand_buf, len);
    parser->instr_buf[parser->instr_buf_index] = instr;
    parser->instr_buf_index++;
    //parser->instr_count++;
}

void parser_add_label(parser* parser, char* label_name){
    label* lab = new_label(parser->instr_buf_index, label_name);
    parser->label_buf[parser->label_buf_index] = lab;
    parser->label_buf_index++;
    //parser->label_count++;
}

void parse_negative_data_value(parser* parser){
    expect(parser, TOK_DEC_NUM, "parse_negative_data_value:1");
    if (accept(parser, TOK_COLON)) {
        if (accept(parser, TOK_DEC_NUM)) {
            for (size_t i = 0; i < atoi(prev_tok(parser, 1)->val); i++)
                variable_add_value(parser->current_var, VAL_NEG_DEC_NUM, prev_tok(parser, 3)->val);
                //values_buf[(*values_buf_index)++] = new_value(VAL_NEG_DEC_NUM, prev_tok(parser, 3)->val);
        } else if (accept(parser, TOK_HEX_NUM)) {
            for (size_t i = 0; i < (size_t)strtol(prev_tok(parser, 1)->val, NULL, 16); i++)
                //values_buf[(*values_buf_index)++] = new_value(VAL_NEG_DEC_NUM, prev_tok(parser, 3)->val);
                variable_add_value(parser->current_var, VAL_NEG_DEC_NUM, prev_tok(parser, 3)->val);
        }
    } else
        variable_add_value(parser->current_var, VAL_NEG_DEC_NUM, prev_tok(parser, 1)->val);
        //values_buf[(*values_buf_index)++] = new_value(VAL_NEG_DEC_NUM, prev_tok(parser, 1)->val);
}

void parse_dec_number_data_value(parser* parser){
    if (accept(parser, TOK_COLON)) {
        if (accept(parser, TOK_DEC_NUM)) {
            for (size_t i = 0; i < atoi(prev_tok(parser, 1)->val); i++)
                variable_add_value(parser->current_var, VAL_DEC_NUM, prev_tok(parser, 3)->val);
                // values_buf[(*values_buf_index)++] = new_value(VAL_DEC_NUM, prev_tok(parser, 3)->val);
        } else if (accept(parser, TOK_HEX_NUM)) {
            for (size_t i = 0; i < (size_t)strtol(prev_tok(parser, 1)->val, NULL, 16); i++)
                variable_add_value(parser->current_var, VAL_DEC_NUM, prev_tok(parser, 3)->val);
                // values_buf[(*values_buf_index)++] = new_value(VAL_DEC_NUM, prev_tok(parser, 3)->val);
        }
    } else
        variable_add_value(parser->current_var, VAL_DEC_NUM, prev_tok(parser, 1)->val);
        //values_buf[(*values_buf_index)++] = new_value(VAL_DEC_NUM, prev_tok(parser, 1)->val);
}

void parse_hex_number_data_value(parser* parser){
    if (accept(parser, TOK_COLON)) {
        if (accept(parser, TOK_DEC_NUM)) {
            //printf("%d", atoi(prev_tok(parser, 1)->val));
            for (size_t i = 0; i < atoi(prev_tok(parser, 1)->val); i++){
                variable_add_value(parser->current_var, VAL_HEX_NUM, prev_tok(parser, 3)->val);
            }
                // values_buf[(*values_buf_index)++] = new_value(VAL_HEX_NUM, prev_tok(parser, 3)->val);
                // printf("%s, %s, %s\n", values_buf[0]->val, values_buf[1]->val, values_buf[2]->val);
        } else if (accept(parser, TOK_HEX_NUM)) {
            for (size_t i = 0; i < (size_t)strtol(prev_tok(parser, 1)->val, NULL, 16); i++)
                //values_buf[(*values_buf_index)++] = new_value(VAL_HEX_NUM, prev_tok(parser, 3)->val);
                variable_add_value(parser->current_var, VAL_HEX_NUM, prev_tok(parser, 3)->val);
        }
    } else
        variable_add_value(parser->current_var, VAL_HEX_NUM, prev_tok(parser, 1)->val);
        //values_buf[(*values_buf_index)++] = new_value(VAL_HEX_NUM, prev_tok(parser, 1)->val);
}

void parse_data_value(parser* parser){
    if (accept(parser, TOK_MINUS))
        parse_negative_data_value(parser);
    else if (accept(parser, TOK_DEC_NUM))
        parse_dec_number_data_value(parser);
    else if (accept(parser, TOK_HEX_NUM))
        parse_hex_number_data_value(parser);
    else if (accept(parser, TOK_IDENTIFIER))
        variable_add_value(parser->current_var, VAL_IDENTIFIER, prev_tok(parser, 1)->val);
        //values_buf[(*values_buf_index)++] = new_value(VAL_IDENTIFIER, prev_tok(parser, 1)->val);
    else 
        throw_unexpected_symbol_exception("parse_data_value:1", parser->current_tok);
}

void parse_data_list(parser* parser){
    do {
        parse_data_value(parser);
    } while(accept(parser, TOK_COMMA));
}

void parse_variable_declaration(parser* parser, char* var_name){
    if(accept(parser, TOK_FULLSTOP)){
        expect(parser, TOK_IDENTIFIER, "parse_variable_declaration:1");
        parser->current_tok = prev_tok(parser, 1);
        parser->lex->tok_buf_index--;
        char* data_type = parser->current_tok->val;
        str_toupper(data_type);
        if((strcmp(data_type, "ASCII") == 0) || (strcmp(data_type, "ASCIIZ") == 0)){
            parser->current_tok = next_tok(parser);
            expect(parser, TOK_STRING, "parse_variable_declaration:2");

            variable_type type = strcmp(data_type, "ASCII") == 0 ? VAR_ASCII : VAR_ASCIIZ;
            parser_add_var(parser, type, var_name);
            parser->current_var = prev_var(parser, 1);

            if(strcmp(data_type, "ASCII") == 0){
                variable_add_value(parser->current_var, VAL_ASCII, prev_tok(parser, 1)->val);
            }
            else variable_add_value(parser->current_var, VAL_ASCIIZ, prev_tok(parser, 1)->val);

        } else if ((strcmp(data_type, "WORD") == 0) || (strcmp(data_type, "HALF") == 0) || (strcmp(data_type, "BYTE") == 0)){
            parser->current_tok = next_tok(parser);
            variable_type type = strcmp(data_type, "WORD") == 0 ? VAR_WORD : strcmp(data_type, "HALF") == 0 ? VAR_HALF : VAR_BYTE;
            parser_add_var(parser, type, var_name);
            parser->current_var = prev_var(parser, 1);

            parse_data_list(parser);

        } else if (strcmp(data_type, "SPACE") == 0){
            parser->current_tok = next_tok(parser);
            if(accept(parser, TOK_DEC_NUM)){
                parser_add_var(parser, VAR_SPACE, var_name);
                parser->current_var = prev_var(parser, 1);
                variable_add_value(parser->current_var, VAL_DEC_NUM, prev_tok(parser, 1)->val);

            } else if (accept(parser, TOK_HEX_NUM)){
                parser_add_var(parser, VAR_SPACE, var_name);
                parser->current_var = prev_var(parser, 1);
                variable_add_value(parser->current_var, VAL_HEX_NUM, prev_tok(parser, 1)->val);

            } else throw_unexpected_symbol_exception("parse_variable_declaration:3", parser->current_tok);
        }
    } else throw_unexpected_symbol_exception("parse_variable_declaration:4", parser->current_tok);
}

void parse_data_section(parser* parser){
    if(accept(parser, TOK_FULLSTOP)){
        if(accept(parser, TOK_IDENTIFIER)){
            if(strcmp(prev_tok(parser, 1)->val, "data")!= 0) throw_unexpected_symbol_exception("parse_data_section:1", parser->current_tok);
            while( parser->current_tok && (parser->current_tok->type != TOK_FULLSTOP)){
                if(accept(parser, TOK_IDENTIFIER)){
                    if(accept(parser, TOK_COLON)) parse_variable_declaration(parser, prev_tok(parser, 2)->val);
                    else throw_unexpected_symbol_exception("parse_data_section:2", parser->current_tok);
                } else throw_unexpected_symbol_exception("parse_data_section:3", parser->current_tok);
            }
        }
    }
}

void parse_register_name(parser* parser){
    if(accept(parser, TOK_DOLLAR_SIGN)){
        if((parser->current_tok->type == TOK_DEC_NUM) || (parser->current_tok->type == TOK_IDENTIFIER)) parser->current_tok = next_tok(parser);
        else throw_unexpected_symbol_exception("parse_register_name:1", parser->current_tok);
    } else throw_unexpected_symbol_exception("parse_register_name:2", parser->current_tok);
}

void parse_operand(parser* parser, operand* operand_buf[], size_t* operand_buf_index){
    
    if (accept(parser, TOK_L_PAREN)) {
        parse_register_name(parser);
        expect(parser, TOK_R_PAREN, "parse_operand:1");
        operand_buf[(*operand_buf_index)++] = new_operand(OP_REG, prev_tok(parser, 2)->val);
    }

    else if (accept(parser, TOK_MINUS)) {
        expect(parser, TOK_DEC_NUM, "parse_operand:2");
        operand_buf[(*operand_buf_index)++] = new_operand(OP_NEG_DEC_NUM, prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_DEC_NUM)) {
        if (accept(parser, TOK_L_PAREN)) {
            parse_register_name(parser);
            expect(parser, TOK_R_PAREN, "parse_operand:3");
            char* inx_reg = NULL;
            sprintf(inx_reg, "%s(%s)", prev_tok(parser, 5)->val, prev_tok(parser, 2)->val);
            operand_buf[(*operand_buf_index)++] = new_operand(OP_INX_REG, inx_reg);
        } else
            operand_buf[(*operand_buf_index)++] = new_operand(OP_DEC_NUM, prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_HEX_NUM))
        operand_buf[(*operand_buf_index)++] = new_operand(OP_HEX_NUM, prev_tok(parser, 1)->val);

    else if (accept(parser, TOK_IDENTIFIER)) {
        if (accept(parser, TOK_PLUS)) {
            expect(parser, TOK_DEC_NUM, "parse_operand:4");
            if (accept(parser, TOK_L_PAREN)) {
                parse_register_name(parser);
                expect(parser, TOK_R_PAREN, "parse_operand:5");
                char* idn_plus_inx_reg = NULL;
                sprintf(idn_plus_inx_reg, "%s+%s(%s)", prev_tok(parser, 2)->val, prev_tok(parser, 5)->val, prev_tok(parser, 7)->val);
                operand_buf[(*operand_buf_index)++] = new_operand(OP_IDN_PLUS_INX_REG, idn_plus_inx_reg);
            } else {
                char* idn_dec_num = NULL;
                sprintf(idn_dec_num, "%s+%s", prev_tok(parser, 3)->val, prev_tok(parser, 1)->val);
                operand_buf[(*operand_buf_index)++] = new_operand(OP_IDN_DEC_NUM, idn_dec_num);
            }
        } else if (accept(parser, TOK_L_PAREN)) {
            parse_register_name(parser);
            expect(parser, TOK_R_PAREN, "parse_operand:6");
            char* idn_inx_reg = NULL;
            sprintf(idn_inx_reg, "%s(%s)", prev_tok(parser, 5)->val, prev_tok(parser, 2)->val);
            operand_buf[(*operand_buf_index)++] = new_operand(OP_IDN_INX_REG, idn_inx_reg);
        } else
            operand_buf[(*operand_buf_index)++] = new_operand(OP_IDN, prev_tok(parser, 1)->val);
    }

    else if (accept(parser, TOK_DOLLAR_SIGN)) {
        
        if ((parser->current_tok->type == TOK_DEC_NUM) || (parser->current_tok->type == TOK_IDENTIFIER)) {
            operand_buf[(*operand_buf_index)++] = new_operand(OP_REG, parser->current_tok->val);
            parser->current_tok = next_tok(parser);
        } else throw_unexpected_symbol_exception("parse_operand:7", parser->current_tok);
    }
    
}   


void parse_operand_list(parser* parser, operand* operand_buf[], size_t* operand_buf_index){
    do{
        parse_operand(parser, operand_buf, operand_buf_index);
    }while(accept(parser, TOK_COMMA));
    
}

void parse_instruction(parser* parser){
    char* opcode = NULL;
    if(accept(parser, TOK_IDENTIFIER)){
        if(accept(parser, TOK_FULLSTOP)){
            if(accept(parser, TOK_IDENTIFIER)) sprintf(opcode, "%s.%s", prev_tok(parser, 3)->val, prev_tok(parser, 1)->val);
            else throw_unexpected_symbol_exception("parse_instruction:1", parser->current_tok);
        } else opcode = prev_tok(parser, 1)->val;

        str_toupper(opcode);
        operand* operand_buf[BUF_MAX];
        size_t operand_buf_index = 0;

        // sycall check
        if(strcmp(opcode, "SYSCALL")==0){
            parser_add_instruction(parser, opcode, operand_buf, operand_buf_index);
        }
        else{
            if(parser->current_tok) parse_operand_list(parser, operand_buf, &operand_buf_index);
            parser_add_instruction(parser, opcode, operand_buf, operand_buf_index);
        }

        
        
    } else throw_unexpected_symbol_exception("parse_instruction:2", parser->current_tok);
}

void parse_text_section(parser* parser){
    //printf("text1: %s\n", parser->var_buf[0]->values[0]->val);
    if(accept(parser, TOK_FULLSTOP)){
        if(accept(parser, TOK_IDENTIFIER)){
            if(strcmp(prev_tok(parser, 1)->val, "text") != 0) throw_unexpected_symbol_exception("parse_text_section:1", parser->current_tok);
            if(accept(parser, TOK_FULLSTOP)){
                if(accept(parser, TOK_IDENTIFIER)){
                    if(strcmp(prev_tok(parser, 1)->val, "globl") != 0) throw_unexpected_symbol_exception("parse_text_section:2", parser->current_tok);
                    if(accept(parser, TOK_IDENTIFIER)) parser->entry_point = prev_tok(parser, 1)->val;
                    else throw_unexpected_symbol_exception("parse_text_section:3", parser->current_tok);
                } else throw_unexpected_symbol_exception("parse_text_section:4", parser->current_tok);
            }
            
            while( parser->current_tok && (parser->current_tok->type != TOK_FULLSTOP)){
                if(accept(parser, TOK_IDENTIFIER)){
                    if(accept(parser, TOK_COLON)){
                        //printf("here: %s\n", parser->var_buf[0]->values[0]->val);
                        parser_add_label(parser, prev_tok(parser, 2)->val);
                    } else{
                        parser->current_tok = prev_tok(parser, 1);
                        parser->lex->tok_buf_index--;
                        parse_instruction(parser);
                        //printf("herea: %s\n", parser->var_buf[0]->values[0]->val);
                    }
                } else throw_unexpected_symbol_exception("parse_text_section:5", parser->current_tok);
            ////printf("here: %s\n", parser->var_buf[0]->values[0]->val);
            }
            ////printf("here: %s\n", parser->var_buf[0]->values[0]->val);
        } else throw_unexpected_symbol_exception("parse_text_section:6", parser->current_tok);
    } else throw_unexpected_symbol_exception("parse_text_section:7", parser->current_tok);
    ////printf("text_section: %s\n", parser->var_buf[0]->values[0]->val);
}

void parser_parse_program(parser* parser){
    if(accept(parser, TOK_FULLSTOP)){
        if(strcmp(parser->current_tok->val, "data") == 0){
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_data_section(parser);
            parse_text_section(parser);
        } else if(strcmp(parser->current_tok->val, "text") == 0){
            parser->current_tok = prev_tok(parser, 1);
            parser->lex->tok_buf_index--;
            parse_text_section(parser);
            parse_data_section(parser);
        }
        if(parser->lex->tok_buf_index!=parser->lex->tok_count) throw_unexpected_symbol_exception("parse_program:1", parser->current_tok);
    } else throw_unexpected_symbol_exception("parse_program:2", parser->current_tok);
}

void parser_reset_buf_indexes(parser* parser){
    parser->var_count = parser->var_buf_index;
    parser->label_count = parser->label_buf_index;
    parser->instr_count = parser->instr_buf_index;
    parser->instr_buf_index = 0;
    parser->label_buf_index = 0;
    parser->var_buf_index = 0;
}


/*

dont use value buffer just append straight to parser var after creation

*/