/*
 * duplicate_check.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * duplicate_check.c is the implementation for a helper module used by sudoku. 
 * It defines all the functions to check for the correct formatting
 * and existence of duplicates in a given solved sudoku puzzle pgm file. It
 * checks for the existence of duplicate values within a row, column,
 * or a 3x3 sub-box within an instance of a UArray2_T that stores the
 * raster of a pgm file.
 *
 */

#include "duplicate_check.h"

const int DIMENSION = 9;

/*
    Function: range_check_intensity
    Purpose: checks if the intensity of the given pixel is within the range 1-9
                to match the sudoku format. Exits with a code of 1 if pixel is
                not in range.
    Inputs: int col - column number of given pixel
            int row - row number of given pixel
            UArray2_T UArray2 - image rasters
            void *val - value of given pixel 
            void *cl - closure
    Returns: N/A
    Expectation: col, row, val, and UArray2 should be non-null. Val
                 should be either 0 or 1. Col and row should be between 0 and
                 UArray2's width and height respectively.
    Notes: will raise CRE if UArray2 is null

*/
void range_check_intensity(int col, int row, UArray2_T UArray2, void *val, 
                           void *cl)
{
        assert(UArray2 != NULL);
        /* apply function checks whether intensity is in the correct range */
        if (*((int *)val) < 1 || *((int *)val) > DIMENSION) {
                exit(EXIT_FAILURE);
        }
        (void) col;
        (void) row;
        (void) cl;
        (void) UArray2;
}

/*
    Function: row_duplicate_check
    Purpose: checks if there are pixels of the same intensity in a given row. 
                Exits with a code of 1 if duplicate pixels exist in a row.
    Inputs:  UArray2_T sudoku_arr - image rasters
    Returns: N/A
    Expectation: sudoku_arr has to be non-null
    Notes: will raise CRE if sudoku_arr is null

*/
void row_duplicate_check(UArray2_T sudoku_arr)
{
        assert(sudoku_arr != NULL);

        int count_arr[9] = {0};
        
        /* Iterates through each row in raster */
        for (int i = 0; i < DIMENSION; i++) {
                for (int j = 0; j < DIMENSION; j++) {
                        /* Populates count_arr with no. of instances */
                        int elem = *(int *)UArray2_at(sudoku_arr, j, i);
                        count_arr[elem - 1]++;
                        /* Exits if instances > 1 (duplicate exists) */
                        if (count_arr[elem - 1] > 1) {
                                exit(EXIT_FAILURE);
                        }
                }
                /* Resets count_arr */
                for (int a = 0; a < DIMENSION; a++) {
                        count_arr[a] = 0;
                }
                    
        }
        
}

/*
    Function: col_duplicate_check
    Purpose: checks if there are pixels of the same intensity in a given 
             column. Exits with a code of 1 if duplicate pixels exist in 
             a column.
    Inputs:  UArray2_T sudoku_arr - image rasters
    Returns: N/A
    Expectation: sudoku_arr has to be non-null
    Notes: will raise CRE if sudoku_arr is null

*/
void col_duplicate_check(UArray2_T sudoku_arr)
{
        assert(sudoku_arr != NULL);
        
        int count_arr[9] = {0};

        /* Iterates through each column in raster */
        for (int i = 0; i < DIMENSION; i++) {
                for (int j = 0; j < DIMENSION; j++) {
                        /* Populates count_arr with no. of instances */
                        int elem = *(int *)UArray2_at(sudoku_arr, i, j);
                        count_arr[elem - 1]++;
                        /* Exits if instances > 1 (duplicate exists) */
                        if (count_arr[elem - 1] > 1) {
                                exit(EXIT_FAILURE);
                        }
                }
                for (int a = 0; a < DIMENSION; a++) {
                        count_arr[a] = 0;
                } 
        }
}

/*
    Function: box_duplicate_check
    Purpose: checks if there are pixels of the same intensity in a given 3 x 3
                box. Exits with a code of 1 if duplicate pixels exist in a box.
    Inputs:  UArray2_T sudoku_arr - image rasters
    Returns: N/A
    Expectation: sudoku_arr has to be non-null
    Notes: will raise CRE if sudoku_arr is null

*/
void box_duplicate_check(UArray2_T sudoku_arr)
{
        assert(sudoku_arr != NULL);

        int count_arr[9] = {0};
        
        /* Iterates through row of 3x3 boxes - increments by 3 */
        for (int i = 0; i < DIMENSION; i += 3) {
                /* Iterates through col of 3x3 boxes */
                for (int j = 0; j < DIMENSION; j += 3) {
                        /* Iterates within 3x3 boxes */
                        for (int k = i; k < i + 3; k++) {
                                for (int l = j; l < j + 3; l++) {
                                        int n = *(int *)UArray2_at
                                                        (sudoku_arr, l, k);
                                        count_arr[n-1]++;
                                        if (count_arr[n-1] > 1) {
                                                exit(EXIT_FAILURE);
                                        } 
                                }
                        }
                        /* Resets count_arr */
                        for (int a = 0; a < DIMENSION; a++) {
                                count_arr[a] = 0;
                        } 
                }        
        }
}