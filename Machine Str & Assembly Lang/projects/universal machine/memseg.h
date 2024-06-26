//*****************************************************************************
//
//                          memseg.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for memseg.c.
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
#include <sys/stat.h>
#include "mem.h"

#include "bitpack.h"
#include "seq.h"

#ifndef MEMSEG_INCLUDED
#define MEMSEG_INCLUDED


void initialize_memory(unsigned int size);
void new_segment(Seq_T segments, unsigned int instr_size);
void renew_segment(Seq_T segments, unsigned int instr_size, uint32_t curr_id);
uint32_t map_segment(Seq_T segments, unsigned int instr_size);
void unmap_segment(Seq_T segments, uint32_t id);
void load_program_segment(Seq_T segments, uint32_t id);
void halt_helper(Seq_T segments);



#endif