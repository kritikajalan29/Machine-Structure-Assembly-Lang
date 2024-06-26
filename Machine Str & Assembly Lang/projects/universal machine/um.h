//*****************************************************************************
//
//                          um.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for um.c .
//
//     Tufts University, Fall 2022
//     CS 40: HW6 UM
//
//     Date: 11/22/2022
//
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "assert.h"
#include "seq.h"
#include "memseg.h"
#include "unpack.h"
#include "operations.h"

#ifndef UM_INCLUDED
#define UM_INCLUDED

extern FILE *open_file(char *fname, char *mode);
extern void read_program(Seq_T segments, FILE *fp, unsigned int size);
extern void unpack_word (Seq_T segments);

#endif