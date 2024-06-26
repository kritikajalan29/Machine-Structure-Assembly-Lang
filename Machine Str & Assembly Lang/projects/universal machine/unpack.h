//*****************************************************************************
//
//                          unpack.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for unpack.c . FILL IN
//
//     Tufts University, Fall 2022
//     CS 40: HW6 UM
//
//     Date: 11/22/2022
//
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "bitpack.h"

#ifndef UNPACK_INCLUDED
#define UNPACK_INCLUDED

extern unsigned int opcode_unpack(uint32_t instruction);
extern unsigned int ra_unpack(uint32_t instruction);
extern unsigned int rb_unpack(uint32_t instruction);
extern unsigned int rc_unpack(uint32_t instruction);
extern unsigned int lv_ra_unpack(uint32_t instruction);
extern unsigned int lv_value_unpack(uint32_t instruction);


#endif