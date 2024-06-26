//*****************************************************************************
//
//                          rgb_cv_convert.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface of rgb_cv_convert.c
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

#ifndef RGB_CV_CONVERT_INCLUDED
#define RGB_CV_CONVERT_INCLUDED

#define CompVideo cv

/* Compression Functions */
extern void RGB_to_CV(int col, int row, A2Methods_UArray2 pixels, void *val, 
                                                                    void *cl);

/* Decompression Functions */
extern void CV_to_RGB(int col, int row, A2Methods_UArray2 pixels, void *val, 
                                                                void *cl);


#undef cv
#endif
