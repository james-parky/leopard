#ifndef __LABEL_H__
#define __LABEL_H__

#include <stddef.h>

typedef struct {
    size_t index;
    char* val;
} label;

label* new_label(size_t index, char* val);
#endif