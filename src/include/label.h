#ifndef __LABEL_H__
#define __LABEL_H__

#include <stddef.h>

typedef struct {
    size_t index;
    char* val;
} label;

/*****************************************************************************
 * new_label: Creates a new label struct using the given arguments and       *
 *            returns a pointer to said label.                               *
 *****************************************************************************/
label* new_label(size_t index, char* val);

#endif