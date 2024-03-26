/*
 * bit2.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * bit2.c  is an interface to support two-dimensional arrays of bits. This 
 * interface can be helpful when dealing with a 'packed' representation of 
 * pixels such as in high resolution images. bit2.c defines a type Bit2_T
 * and creates the functions required to manipulate a two-dimensional bitmap.
 *
 */

#include "bit2.h"
#include <assert.h>
#include "bit.h"

#define T Bit2_T
struct T {
        int width;
        int height;
        Bit_T bitmap;
};

/*
    Function: Bit2_new
    Purpose: Creates a new Bit2_T of given width and height
    Expectation: width and height parameters have to be non-null and positive
    Creates a new Bit2_T object.

*/
T Bit2_new(int width, int height) { 

        assert(width >= 0 && height >= 0);
        T bit2;
        NEW0(bit2);
        if (width >= 0 && height >= 0) {
                bit2->width = width;
                bit2->height = height;
                bit2->bitmap = Bit_new(width * height);
        }
        return bit2;
}

/*
    Function: Bit2_free
    Purpose: Deallocates memory used for Bit2
    Expectation: Bit2 and *Bit2 parameters have to be non-null.

*/
void Bit2_free(T *bit2) {
        assert(bit2 && *bit2);
        Bit_free(&((*bit2)->bitmap));
        FREE(*bit2);

}

/*
    Function: Bit2_height
    Purpose: Returns the height of the given Bit2_T
    Expectation: Bit2 parameter has to be non-null

*/
int Bit2_height(T bit2) {
        assert (bit2);
        return bit2->height;
}

/*
    Function: Bit2_width
    Purpose: Returns the width of the given Bit2_T
    Expectation: Bit2 parameter has to be non-null

*/
int Bit2_width(T bit2) {
        assert (bit2);
        return bit2->width;
}

/*
    Function: Bit2_get
    Purpose: Returns the value of the Bit2_T at given col and row
    Expectation: Bit2, col, and row parameters have to be non-null. Col and
    row have to be positive and within the dimensions of Bit2.

*/
int Bit2_get(T Bit2, int col, int row) {
    
        assert(Bit2);
        assert(col >= 0 && row >= 0 && col < Bit2->width && 
               row < Bit2->height);
        return (Bit_get(Bit2->bitmap, row * (Bit2->width) + col));
}

/*
    Function: Bit2_put
    Purpose: Puts the value of the Bit2_T at given col and row and returns
             the previous value at those coordinates.
    Expectation: Bit2, col, row, and bit parameters have to be non-null. 
                 Col and row have to be positive and within the dimensions 
                 of Bit2. Bit has to be 0 or 1.

*/
int Bit2_put(T Bit2, int col, int row, int bit) {
    
        assert(Bit2);
        assert(col >= 0 && row >= 0 && col < Bit2->width && 
               row < Bit2->height);
        assert(bit == 0 || bit == 1);
        int prev = Bit_get(Bit2->bitmap, row * (Bit2->width) + col);
        Bit_put(Bit2->bitmap, (row * (Bit2->width) + col), bit);
        return prev;
    
}

/*
    Function: Bit2_map_row_major
    Purpose: It traverses through the Bit2 one row at a time and performs 
             the function provided for each element of the Bit2
    Expectation: Bit2 parameter should be non-null

*/
void Bit2_map_row_major(T Bit2, void apply(int col, int row, T Bit2,
                                  int val, void *cl), void *cl) {

        assert(Bit2);
        size_t width = Bit2->width;
        for (size_t i = 0; i < (width * Bit2->height); i++) {
                /* 'i % width' returns the col and 'i / width' returns
                    the row from the 1D bitmap */
                apply((i%width), (i/width), Bit2, Bit_get(Bit2->bitmap, i),
                        cl);
        }
}

/*
    Function: Bit2_map_col_major
    Purpose: It traverses through the Bit2 one column at a time and performs
             the function provided for each element of the Bit2
    Expectation: Bit2 parameter should be non-null

*/
void Bit2_map_col_major(T Bit2, void apply(int col, int row, T Bit2,
                                  int val, void *cl), void *cl) {
                                    
        assert(Bit2);
        size_t width = Bit2->width;
        size_t dimension = width * Bit2->height;
        /* Iterates through columns */
        for (size_t i = 0; i < width; i++) {
                /* Increments by width to reach correct index in next row */
                for (size_t j = i; j < dimension; j += width) {
                        apply ((j%width), (j/width), Bit2, 
                                Bit_get(Bit2->bitmap, j), cl);
                }
        } 
}