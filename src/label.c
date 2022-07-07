#include "label.h"
#include <stdlib.h>

label* new_label(size_t index, char* val){
    label* lab = malloc(sizeof(label));
    lab->index = index;
    lab->val = val;
    return lab;
}