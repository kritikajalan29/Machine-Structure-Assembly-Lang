//*****************************************************************************
//
//                          file_io.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface of file_io.c
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "pnm.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "mem.h"
#include "arith40.h"
#include "bitpack.h"

#ifndef FILE_IO_INCLUDED
#define FILE_IO_INCLUDED

/* Compression Functions */
extern Pnm_ppm trim(Pnm_ppm original);
extern void write_compressed(A2Methods_UArray2 words_array, 
                             A2Methods_T methods);

/* Decompression Functions */
extern Pnm_ppm read_header(FILE *input);
extern void write_decompressed(Pnm_ppm image);
extern A2Methods_UArray2 codeword_reader(int height, int width, FILE *input, 
                                                        A2Methods_T methods);



#endif