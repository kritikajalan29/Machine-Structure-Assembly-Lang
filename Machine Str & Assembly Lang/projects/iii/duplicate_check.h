/*
 * duplicate_check.h
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * duplicate_check.h is the interface for a helper module used by sudoku. 
 * It declares all the functions to check for the correct formatting
 * and existence of duplicates in a given solved sudoku puzzle pgm file.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "uarray2.h"

#ifndef DUPLICATE_CHECK_INCLUDED
#define DUPLICATE_CHECK_INCLUDED

void range_check_intensity(int col, int row, UArray2_T UArray2, void *val, 
                           void *cl);
void row_duplicate_check(UArray2_T sudoku_arr);
void col_duplicate_check(UArray2_T sudoku_arr);
void box_duplicate_check(UArray2_T sudoku_arr);

#endif