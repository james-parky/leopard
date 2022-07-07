#include "transpiler.h"
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

void transpile_file(FILE* file){
    lexer* lex = lexer_init(file);
    lexer_lex(lex);
    parser* parser = parser_init(lex);
    parser_parse_program(parser);
}