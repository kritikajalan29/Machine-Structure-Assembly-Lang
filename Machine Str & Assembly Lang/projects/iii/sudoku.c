/*
 * sudoku.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * sudoku.c is a test program that functions as a predicate. It makes use of 
 * the uarray2 interface to store a sudoku puzzle in the form of a pgm. Using
 * the functions provided by the uarray2 interface, we check whether the 
 * provided pgm is indeed a correctly solved sudoku puzzle. The program returns
 * 0 if the above is true, else it exits with a code of 1.
 *
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <uarray.h>

#include "pnmrdr.h"
#include "except.h"
#include "uarray2.h"
#include "duplicate_check.h"

const int DIM = 9;

static FILE *open_file(char *fname, char *mode);
static void pgm_reader(FILE *fp);

/*
    Function: main
    Purpose: deals with file I/O and calls pgm_reader
    Inputs: int argc, char *argv[]
               argc is the number of arguments in the command line
               and argv holds the names of the commands
    Returns: 0
    Expectation: argc should be less than equal to 2.
    Notes: raise CRE if argc > 2. raises CRE if file not closed

*/
int main(int argc, char *argv[])
{
        assert(argc <= 2);
        if (argc == 1) {
                pgm_reader(stdin);
        }
        /* Open file and call driver code */
        else if (argc == 2) {
                FILE *file = open_file(argv[1], "r");
                pgm_reader(file);
                int check_close = fclose(file); 
                assert(check_close == 0);
        }
        return 0;
}

/*
    Function: open_file
    Purpose: opens file of given name in specified mode
    Inputs: a pointer to fname and mode.
               fname is the name of the file and mode is the mode of opening 
               (reading, writing etc.)
    Returns: Pointer to a filestream object pointing to open file/stdin
    Expectation: filestream fp cannot be null
    Notes: raises CRE if filestream fp is null

*/
static FILE *open_file(char *fname, char *mode) 
{
        FILE *fp = fopen(fname, mode);
        assert(fp != NULL);
        return fp;
}

/*
    Function: pgm_reader
    Purpose: uses the pnmrdr reader to check if supplied file is a pgm image in
                the correct format. It also populates a UArray_T instance 
                with the image rasters.
    Inputs: a pointer to a filestream object which points to the open
               file
    Returns: N/A
    Expectation: filestream has to contain a pgm
    Notes: raises CRE if filestream does not have a pgm

*/
static void pgm_reader(FILE *fp)
{
        /* Extracts header information and populates local vars */
        Pnmrdr_T reader = Pnmrdr_new(fp);       
        Pnmrdr_mapdata data = Pnmrdr_data(reader);
        UArray2_T raster = UArray2_new(DIM, DIM, 4);
        assert(data.type == 2);
        int height = data.height;
        int width = data.width;
        int maxval = data.denominator;

        /* Exits if header info inconsistent with solved sudoku puzzle */
        if (height != DIM || width != DIM || maxval != DIM) {
                exit(EXIT_FAILURE);
        }

        /* Populates UArray2_T instance with image raster */
        for (int i = 0; i < DIM; i++) {
                for (int j = 0; j < DIM; j++) {
                         *((int *)UArray2_at(raster, j, i)) = 
                                                Pnmrdr_get(reader);
                } 
        }

        /* Calls functions to check for correct solved sudoku format */
        UArray2_map_row_major(raster, range_check_intensity, NULL);
        row_duplicate_check(raster);
        col_duplicate_check(raster);
        box_duplicate_check(raster);
        
        /* Calls functions to free memory allocated*/
        UArray2_free(&raster);
        Pnmrdr_free(&reader);
        
}