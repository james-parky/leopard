#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "transpiler.h"


int main(int argc, char* argv[]){
    if(argc < 2) {
        fprintf(stderr, "Too few arguements supplied.\n");
        exit(1);
    }
    char* file_name = argv[1];
    FILE* file = fopen(file_name, "r");
    if(!file) {
        fprintf(stderr, "Specified file could not be opened.\n");
        exit(1);
    }
    transpile_file(file);
}