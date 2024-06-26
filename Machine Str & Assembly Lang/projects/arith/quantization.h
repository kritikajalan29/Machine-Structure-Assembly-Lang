//*****************************************************************************
//
//                          quantization.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface of quantization.c
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
#include "structs.h"

#ifndef QUANTIZATION_INCLUDED
#define QUANTIZATION_INCLUDED

#define CompVideo cv


/* Compression Functions */
extern void chroma_quantization(int width, int height, 
                                A2Methods_UArray2 cv_pixels, 
                                A2Methods_UArray2 quant_pixels, 
                                A2Methods_T methods);
extern void dct(int col, int row, A2Methods_UArray2 q_blocks, void *val, 
                                                                void *cl);
extern int scaling(double to_scale, int factor);                            

/* Decompression Functions */
extern void inverse_chroma_quantization(A2Methods_UArray2 q_blocks, 
                                        A2Methods_UArray2 cv_pixels, 
                                        A2Methods_T methods);
extern void inverse_dct(int col, int row, A2Methods_UArray2 q_blocks, 
                                                        void *val, void *cl);

#undef cv
#endif
