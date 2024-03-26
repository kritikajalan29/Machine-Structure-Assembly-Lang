/*
 * uarray2.h
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * uarray2.h is the interface for a 2D version of Hanson's UArray_T. 
 * It declares all the functions a UArray2_T would require for full
 * functionality.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "uarray.h"
#include "mem.h"

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

extern T UArray2_new(int width, int height, int size);
extern void UArray2_free(T *uarray2);
extern int UArray2_height(T uarray2);
extern int UArray2_width(T uarray2);
extern int UArray2_size(T uarray2);
extern void *UArray2_at(T uarray2, int col, int row);
extern void UArray2_map_row_major(T UArray2, void apply(int col, int row, 
                                  T UArray2, void *val, void *cl), void *cl);
extern void UArray2_map_col_major(T UArray2, void apply(int col, int row,
                                  T UArray2, void *val, void *cl), void *cl);

#undef T
#endif
