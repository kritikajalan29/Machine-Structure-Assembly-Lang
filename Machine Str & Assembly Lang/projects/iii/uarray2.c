/*
 * uarray2.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * uarray2.c is an adaptation of Hanson's unboxed-array abstraction to support
 * two-dimensional arrays. It defines the type UArray2_T and the implements
 * the necessary functions for two-dimensional arrays to work.
 *
 */

#include "uarray2.h"
#include <assert.h>

#define T UArray2_T

struct T {

        int height;
        int width;
        int size;
        UArray_T array;

}; 

/*
    Function: UArray2_new
    Purpose: Creates a new uarray of given width, height and size
    Inputs: int width - given width used to initialise UArray2_T
            int height - given height used to initialise UArray2_T
            int size - given size used to initialise UArray2_T
    Returns: N/A
    Expectation: width, height, and size parameters have to be non-null. Size
                 has to be positive. Creates a new UArray2 object.

*/
T UArray2_new(int width, int height, int size) { 

        assert(width >= 0 && height >= 0 && size > 0);
        T arr;
        NEW0(arr);
        if (width >= 0 && height >= 0) {
                arr->width = width;
                arr->height = height;
                arr->size = size;
                arr->array = UArray_new((width * height), 
                                        (width * height * size));
        }
        return arr;
}

/*
    Function: UArray2_free
    Purpose: Deallocates memory used for uarray
    Inputs: T *uarray2 - an instance of uarray
    Returns: N/A
    Expectation: uarray2 and *uarray2 parameters have to be non-null.

*/
void UArray2_free(T *uarray2) {
        assert(uarray2 && *uarray2);
        UArray_free(&((*uarray2)->array));
        FREE(*uarray2);
}

/*
    Function: UArray2_height
    Purpose: Returns the height of the given uarray
    Inputs: T *uarray2 - an instance of uarray
    Returns: height of given uarray
    Expectation: uarray2 parameter has to be non-null

*/
int UArray2_height(T uarray2) {
        assert (uarray2);
        return uarray2->height;
}

/*
    Function: UArray2_width
    Purpose: Returns the width of the given uarray
    Inputs: T *uarray2 - an instance of uarray
    Returns: width of given uarray
    Expectation: uarray2 parameter has to be non-null

*/
int UArray2_width(T uarray2) {
        assert (uarray2);
        return uarray2->width;
}

/*
    Function: UArray2_size
    Purpose: Returns the size of the given uarray
    Inputs: T *uarray2 - an instance of uarray
    Returns: size of given uarray
    Expectation: uarray2 parameter has to be non-null

*/
int UArray2_size(T uarray2) {
        assert (uarray2);
        return uarray2->size;
}

/*
    Function: UArray2_at
    Purpose: Returns a pointer to uarray element at position i
    Inputs: int col - column number of an index
            int row - row number of an index
            UArray2_T UArray2 - an instance of uarray
    Returns: a pointer to uarray element at position i       
    Expectation: col has to be between 0 and one less than the number of
                 columns in the UArray2; row has to be be between 0 and one 
                 less than the number of rows in the UArray2

*/
void *UArray2_at(T uarray2, int col, int row) {
        assert(uarray2);
        assert(col >= 0 && row >= 0 && col < uarray2->width && 
                row < uarray2->height);
        /* 2nd argument is the formula that maps our 2D array to a 1D array */
        return (UArray_at(uarray2->array, row * (uarray2->width) + col));
}

/*
    Function: UArray2_map_row_major
    Purpose: It traverses through the uarray one row at a time and performs 
             the function provided by for each element of the uarray
    Inputs: UArray2_T UArray2 - an instance of uarray
            an apply function
            void *cl - closure argument
    Returns: N/A  
    Expectation: uarray2 parameter should be non-null

*/
void UArray2_map_row_major(T UArray2, void apply(int col, int row, 
                                  T UArray2, void *val, void *cl), void *cl) {
        size_t width = UArray2->width; 
        size_t dimension = width * (UArray2->height);
        /* Iterates through UArray2 elems and applies 'apply' to each elem */
        for (size_t i = 0; i < dimension; i++) {
                /* 'i % width' returns the col from the 1D array
                    and 'i / width' returns the row from the 1D array */
                apply ((i%width), (i/width), UArray2, 
                        UArray_at(UArray2->array, i), cl); 
        }
}

/*
    Function: UArray2_map_col_major
    Purpose: It traverses through the uarray one column at a time and performs
             the function provided by for each element of the uarray
             the function provided by for each element of the uarray
    Inputs: UArray2_T UArray2 - an instance of uarray
            an apply function
            void *cl - closure argument
    Returns: N/A 
    Expectation: uarray2 parameter should be non-null

*/
void UArray2_map_col_major(T UArray2, void apply(int col, int row,
                                  T UArray2, void *val, void *cl), void *cl) {

        size_t width = UArray2->width; 
        size_t dimension = width * (UArray2->height);
        /* Iterates through columns */
        for (size_t i = 0; i < width; i++) {
        /* Increments by width to get to appropriate index in next row */
                for (size_t j = i; j < dimension; j += width) {
                        apply ((j%width), (j/width), UArray2, 
                                UArray_at(UArray2->array, j), cl);
                        
                }
        }
}