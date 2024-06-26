//*****************************************************************************
//
//                          decompress_image.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for decompress_image.c
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//****************************************************************************

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
// #include "compress_image.h"


#ifndef DECOMPRESS_IMAGE_INCLUDED
#define DECOMPRESS_IMAGE_INCLUDED

#define CompVideo cv
typedef struct cv *cv;

typedef struct quant *quant;

typedef struct closure *closure;
// typedef struct cl_dct *cl_dct

extern void decompress(FILE *input);

extern Pnm_ppm read_header(FILE *input);
extern void write_decompressed(Pnm_ppm image);
extern void CV_to_RGB(int col, int row, A2Methods_UArray2 pixels, void *val, 
                                                                void *cl);
extern void inverse_chroma_quantization(A2Methods_UArray2 q_blocks, 
                                        A2Methods_UArray2 cv_pixels, 
                                        A2Methods_T methods);
extern void inverse_dct(int col, int row, A2Methods_UArray2 q_blocks, 
                                                        void *val, void *cl);
extern quant bit_unpack_word(uint32_t code_word);
extern void *bit_unpack(A2Methods_UArray2 words_array, A2Methods_T methods);
extern A2Methods_UArray2 codeword_reader(int height, int width, FILE *input, 
                                                        A2Methods_T methods);


#undef cv
#endif