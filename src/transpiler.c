#include "transpiler.h"
#include <stdio.h>
#include "lexer.h"

void transpile_file(FILE* file){
    lexer* lex = lexer_init(file);
    lexer_lex(lex);
}