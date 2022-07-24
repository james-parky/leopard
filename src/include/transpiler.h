#ifndef __TRANSPILER_H__
#define __TRANSPILER_H__

#include <stdio.h>

#include "parser.h"
#include "instruction.h"

typedef struct {
    char* in_file_name, *out_file_name;
    FILE* out;
    parser* parser;
} transpiler;

/*****************************************************************************
 * transpiler_init: Creates a new transpiler struct using the given          *
 *                  arguments and returns a pointer to said transpiler.      *
 *****************************************************************************/
transpiler* transpiler_init(parser* parser, char* in_file_name, FILE* out, 
                                char* out_file_name);

/*****************************************************************************
 * transpile_file: Lexes the full given MIPS32 source code file. Parses the  *
 *                 entire token buffer created by the lex. Generates new     *
 *                 assembly code in either x86_64 or aarch64 depending on    *
 *                 the users machine or the given argument.                  *
 *****************************************************************************/
void transpile_file(FILE* in, char* in_file_name, FILE* out, 
                        char* out_file_name);

void write_instr(transpiler* transp, instruction* instr);
#endif