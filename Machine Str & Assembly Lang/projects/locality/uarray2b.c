//****************************************************************
//
//                          urray2b.c
//
//
//     Author: Kritika Jalan and Arya Prasad
//     
//     Purpose: Creates a two - dimensional blocked array
//              using the UArray and UArray2 interfaces
//
//     Tufts University, Spring 2022
//     CS 40: HW3 locality
//
//     Date: 10/7/2022
//
//****************************************************************


#include <uarray2b.h>
#include <uarray2.h>
#include <uarray.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include "mem.h"
#include "math.h"

#define T UArray2b_T 
/*
    Struct T represents the elements of a UArray2b
*/
struct T {
        int height;
        int width;
        int size;
        int blocksize;
        int width_arr;   //actual width of the array using blocks
        int height_arr;  //actual height of the array using blocks
        UArray2_T twoDim;
};


/*
    Function: UArray2b_new
    Purpose: Creates a new uarray2b of given width, height and size
    Inputs: int width - given width used to initialise UArray2b_T
            int height - given height used to initialise UArray2b_T
            int size - given size used to initialise UArray2b_T
            int blocksize - given size to 
    Returns: A 2D array that stores a 1D array. Each 1D array represents
                cells within a block.
    Expectation: width, height, and size parameters have to be non-null. Size
                 has to be positive. Creates a new UArray2 object.

*/
T UArray2b_new (int width, int height, int size, int blocksize) 
{
        assert(width >= 0 && height >= 0 && size > 0 && blocksize >=0);
        T blockedArray;
        UArray_T *oneDim;

        NEW0(blockedArray);
        if (width >= 0 && height >= 0 && blocksize >= 0) {
                blockedArray->width = width;
                
                /* if image width is not a multiple of the blocksize, 
                we need an extra block else we divide width/blocksize */
                if (width % blocksize != 0) {
                        width = (width / blocksize) + 1;
                        blockedArray->width_arr = width;
                } else {
                        blockedArray->width_arr = width/blocksize;
                }
                /* if image height is not a multiple of the blocksize, 
                we need an extra block else we divide height/blocksize */
                blockedArray->height = height;
                if (height % blocksize != 0) {
                        height = (height / blocksize) + 1;
                        blockedArray->height_arr = height;
                } else {
                        blockedArray->height_arr = height/blocksize;
                }

                blockedArray->size = size;
                blockedArray->blocksize = blocksize;
                blockedArray->twoDim = UArray2_new(blockedArray->width_arr, 
                        blockedArray->height_arr, sizeof(UArray_T));
        }

        /* intialize 1D array for each index of the 2D array */
        for (int col = 0; col < blockedArray->width_arr; col++) {
                for (int row = 0; row <  blockedArray->height_arr; row++) {
                        oneDim = UArray2_at(blockedArray->twoDim, col, row);
                        /* each elemment of 1D array will store a pixel */
                        *oneDim = UArray_new((blocksize * blocksize), size);
                } 
        }

        return blockedArray;
        
}



/*
    Function: UArray2b_new_64K_block
    Purpose: Creates a new uarray2b of given width, height and size
                Each block can store upto 64KB of RAM
    Inputs: int width - given width used to initialise UArray2b_T
            int height - given height used to initialise UArray2b_T
            int size - given size used to initialise UArray2b_T 
    Returns: A 2D array that stores a 1D array. Each 1D array represents a
                block.
    Expectation: width, height, and size parameters have to be non-null. Size
                 has to be positive. Creates a new UArray2 object.

*/
T UArray2b_new_64K_block(int width, int height, int size)
{
        assert(width >= 0 && height >= 0 && size > 0);
        T blockedArray;
        int blocksize;
        /*(blocksize)^2 * size = size of the whole block*/
        /*size of the whole block = 64000*/
        blocksize = sqrt(64000 / size);
        blockedArray = UArray2b_new(width, height, size, blocksize);

        return blockedArray;
}

/*
    Function: UArray2b_free
    Purpose: Deallocates memory used for uarray2b
                - uarray_t instance in every uarray2 is freed
                - uarray2 is freed
    Inputs: T *array2b - an instance of uarray2b
    Returns: N/A
    Expectation: uarray2b that we pass in has to be non-null

*/
extern void UArray2b_free (T *array2b)
{
        assert(array2b && *array2b);
        UArray_T *oneDim;
        
        /* free 1D array from each index of the 2D array */ 
        for (int col = 0; col < ((*array2b)->width_arr); col++) {
                for (int row = 0; row < ((*array2b)->height_arr); row++) {
                        oneDim = UArray2_at((*array2b)->twoDim, col, row);
                        /* free 1D array */
                        UArray_free(oneDim);
                } 
        }

        UArray2_free(&((*array2b)->twoDim));
        FREE (*array2b);
}

/*
    Function: UArray2b_width
    Purpose: Returns the width of the given uarray2b
    Inputs: T *uarray2b - an instance of uarray2b
    Returns: width of given uarray2b
    Expectation: uarray2b parameter has to be non-null

*/
extern int UArray2b_width (T array2b)
{
        assert(array2b);
        return array2b->width;
}

/*
    Function: UArray2b_height
    Purpose: Returns the height of the given uarray2b
    Inputs: T *uarray2b - an instance of uarray
    Returns: height of given uarray2b
    Expectation: uarray2b parameter has to be non-null

*/
extern int UArray2b_height (T array2b) 
{
        assert(array2b);
        return array2b->height;
}

/*
    Function: UArray2b_size
    Purpose: Returns the size of the given uarray2b
    Inputs: T *uarray2b - an instance of uarray2b
    Returns: size of given uarray2b
    Expectation: uarray2b parameter has to be non-null

*/
extern int UArray2b_size (T array2b)
{
        assert(array2b);
        return array2b->size;
}

/*
    Function: UArray2b_blocksize
    Purpose: Returns the blocksize of the given uarray2b
    Inputs: T *uarray2b - an instance of uarray2b
    Returns: blocksize of given uarray2b
    Expectation: uarray2b parameter has to be non-null
*/
extern int UArray2b_blocksize(T array2b)
{
        assert(array2b);
        return array2b->blocksize;
}

/*
    Function: UArray2b_blocksize
    Purpose: Returns the blocksize of the given uarray2b
    Inputs: T *uarray2b - an instance of uarray2b
    Returns: blocksize of given uarray2b
    Expectation: uarray2b parameter has to be non-null
*/
extern void *UArray2b_at(T array2b, int column, int row) 
{
        assert(array2b);
        UArray_T *oneDim;
        int bsize = array2b->blocksize;

        // CRE when tr to access 'empty' cells (in edge blocks)
        assert(column < array2b->width && row < array2b->height);
        oneDim = UArray2_at((array2b)->twoDim, (column/bsize), 
                        (row/bsize));
        int index1D = bsize * 
                        (row % bsize) + (column % bsize);
        return UArray_at(*oneDim, index1D);   
                                               
        
}
/*
    Function: UArray2b_blocksize
    Purpose: Returns the blocksize of the given uarray2b
    Inputs: T *uarray2b - an instance of uarray2b
    Returns: blocksize of given uarray2b
    Expectation: uarray2b parameter has to be non-null
*/
extern void UArray2b_map(T array2b,void apply(int col, int row, T array2b, 
                                void *elem, void *cl),void *cl)
{
        assert(array2b);
        UArray_T *oneDim;
        int col, row;

        // traversing the 2D UArray
        for (int col_arr = 0; col_arr < (array2b->width_arr); col_arr++) {
                for(int row_arr = 0; row_arr < (array2b->height_arr); 
                        row_arr++) {
                        oneDim = UArray2_at(array2b->twoDim, 
                                col_arr, row_arr);
                        // for each cell inside a block
                        for (int block_index = 0; block_index < 
                                UArray_length(*oneDim); block_index ++) {

                                /*convert from block to whole array*/
                                col = (col_arr * array2b->blocksize) +
                                (block_index % array2b->blocksize);
                                row = (row_arr * array2b->blocksize) +
                                (block_index / array2b->blocksize);

                                /*check for empty cells*/
                                if (col >= array2b->width ||
                                        row >= array2b->height) {
                                        continue;
                                } else {
                                        apply(col, row, array2b,
                                        UArray_at(*oneDim, block_index), cl);
                                }
                                
                        }
                             
                }
        }
        
}