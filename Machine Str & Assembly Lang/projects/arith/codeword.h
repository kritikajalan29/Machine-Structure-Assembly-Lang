//*****************************************************************************
//
//                          codeword.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface of codeword.c
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

#include "quantization.h"

#ifndef CODEWORD_INCLUDED
#define CODEWORD_INCLUDED


/* Compression Functions */
extern uint64_t bit_pack_word(quant q_blocks);
extern void *bit_pack(closure cls);

/* Decompression Functions */
extern quant bit_unpack_word(uint32_t code_word);
extern void *bit_unpack(A2Methods_UArray2 words_array, A2Methods_T methods);

#endif
