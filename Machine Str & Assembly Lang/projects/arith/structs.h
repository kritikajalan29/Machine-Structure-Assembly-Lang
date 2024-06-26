//*****************************************************************************
//
//                          structs.h
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: interface for structs.c . It also defines key structs used in
//              the process of compressing and decompressing image files.
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

#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"

#ifndef STRUCTS_INCLUDED
#define STRUCTS_INCLUDED

static const int DENOMINATOR = 255.0;

/*
    Struct: CompVideo
    Purpose: Represents the pixels of an image
    Members: double Y → luminance of the pixel
            double pb → blue
            double pr → red
*/
typedef struct cv {
        double Y, pb, pr;
} *cv;

/*
    Struct: quant
    Purpose: Represents 2x2 blocks of pixels of an image
    Members: double y1 → luminance of pixel 1
             double y2 → luminance of pixel 2
             double y3 → luminance of pixel 3
             double y4 → luminance of pixel 4
            unsigned int avg_pb → avg pb of the 4 pixels
            unsigned int avg_pr → avg pr of the 4 pixels
            unsigned int a → average luminance of the 4 pixels
            signed int b → function of the luminance of the 4 pixels
            signed int c → function of the luminance of the 4 pixels
            signed int d → function of the luminance of the 4 pixels
*/
typedef struct quant {
        unsigned int avg_pb, avg_pr, a;
        double y1, y2, y3, y4;
        signed int b, c, d;
} *quant;

/*
    Struct: closure
    Purpose: to be passed as a closure to apply functions with a UArray2 and 
             an A2Methods instance
    Members: A2Methods_UArray2 arr → array to be passed into the closure
            A2Methods_T methods → instance of A2Methods to be passed into
                                  the closure
*/
typedef struct closure {
        A2Methods_UArray2 arr;
        A2Methods_T methods;
} *closure;

extern double bound_check(double value, double bound); 

#endif
