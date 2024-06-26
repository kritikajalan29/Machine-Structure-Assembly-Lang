//*****************************************************************************
//
//                          operations.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for operations.c .
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
#include "seq.h"

#include "memseg.h"

#ifndef OPERATIONS_INCLUDED
#define OPERATIONS_INCLUDED


void execute_operation(unsigned int opcode, unsigned int ra, unsigned int rb, 
                       unsigned int rc, Seq_T segments, uint32_t *pc);

void execute_lv(unsigned int ra, unsigned int value);

#endif