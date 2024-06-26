#include "stdio.h"
#include "stdint.h"

#ifndef STRUCTS_INCLUDED
#define STRUCTS_INCLUDED

typedef struct segments {
        int curr_size, capacity;
        uint32_t *arr[];
} *segments;

typedef struct unmapped {
        int curr_size;
        uint32_t id[];
} *unmapped;

typedef struct size_struct {
        int curr_size;
        uint32_t sizes[];
} *size_struct;

#endif