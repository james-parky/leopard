#include "label.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

label* new_label(size_t index, char* val) {
    label* lab = malloc(sizeof(label));
    lab->index = index;
    // lab->val = malloc(sizeof(char) * strlen(val) + 1);
    // memcpy(lab->val, val, strlen(val));
    // lab->val[strlen(val)] = '\0';
    lab->val = val;
    return lab;
}