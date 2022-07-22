#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transpiler.h"

int main(int argc, char* argv[]) {
    if(argc < 3) fprintf(stderr, "Too few arguments supplied.\n");
    if(argc > 3) fprintf(stderr, "Too many arguments supplied.\n");

    char* in_file_name = argv[1];
    char* out_file_name = argv[2];

    FILE* in = fopen(in_file_name, "r");
    FILE* out = fopen(out_file_name, "w+");

    if(!in) fprintf(stderr, "%s could not be opened.\n", in_file_name);
    if(!out) fprintf(stderr, "%s could not be opened.\n", out_file_name);

    transpile_file(in, in_file_name, out, out_file_name);
    return 0;
}




                                                                                                                                                                                                                                                                                                                             
