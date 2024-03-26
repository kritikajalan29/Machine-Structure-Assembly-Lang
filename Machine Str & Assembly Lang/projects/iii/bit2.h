/*
 * bit2.h
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * bit2.h is the interface for a 2D version of Hanson's Bit_T. 
 * It declares all the functions a Bit2_T would require for full
 * functionality.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mem.h"

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

#define T Bit2_T
typedef struct T *T;


extern T Bit2_new(int width, int height);
extern void Bit2_free(T *Bit2);

extern int Bit2_width(T Bit2);
extern int Bit2_height(T Bit2);

extern int Bit2_get(T Bit2, int col, int row);
extern int Bit2_put(T Bit2, int col, int row, int bit);

extern void Bit2_map_row_major(T Bit2, void apply(int col, int row, T Bit2,
                                  int val, void *cl), void *cl);
extern void Bit2_map_col_major(T Bit2, void apply(int col, int row, T Bit2,
                                  int val, void *cl), void *cl);

#undef T
#endif