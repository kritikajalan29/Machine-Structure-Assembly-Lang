//*****************************************************************************
//
//                          compress_image.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for compress_image.c
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <string.h>
// #include <math.h>

// #include "pnm.h"
// #include "uarray2.h"
// #include "uarray2b.h"
// #include "a2methods.h"
// #include "a2plain.h"
// #include "a2blocked.h"
// #include "mem.h"
// #include "arith40.h"
#include "bitpack.h"
#include "decompress_image.h"

#ifndef COMPRESS_IMAGE_INCLUDED
#define COMPRESS_IMAGE_INCLUDED

#define CompVideo cv
// typedef struct cv *cv;

// typedef struct quant *quant;

extern void compress(Pnm_ppm original, A2Methods_T methods);

extern Pnm_ppm trim(Pnm_ppm original);
extern void RGB_to_CV(int col, int row, A2Methods_UArray2 pixels, void *val, 
                      void *cl);
extern void chroma_quantization(int width, int height, 
                                A2Methods_UArray2 cv_pixels, 
                                A2Methods_UArray2 quant_pixels, 
                                A2Methods_T methods);
extern void dct(int col, int row, A2Methods_UArray2 q_blocks, void *val, 
                void *cl);
extern uint64_t bit_pack_word(quant q_blocks);
extern void *bit_pack(closure cls);
extern void write_compressed(A2Methods_UArray2 words_array, A2Methods_T methods);
extern int scaling(double to_scale, int factor);
extern double bound_check(double value, double bound);


#undef cv
#endif