//****************************************************************
//
//                          a2plain.c
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

#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

typedef A2Methods_UArray2 A2;

/*
    Function: new
    Purpose: Create unblocked 2D array
    Inputs: int width, height, size - dimensions and info
            about image
    Returns: unblocked 2D array 
    Expectation: does not access out of bounds cells 

*/
static A2 new(int width, int height, int size)
{
  return UArray2_new(width, height, size);
}

/*
    Function: new_with_blocksize
    Purpose: Create unblocked 2D array with blocksize
    Inputs: int width, height, size, blocksize - dimensions and info
            about image
    Returns: unblocked 2D array 
    Expectation: does not access out of bounds cells 

*/
static A2 new_with_blocksize(int width, int height, int size, 
                                              int blocksize)
{
  (void) blocksize;
  return UArray2_new(width, height, size);
}

/*
    Function: a2free
    Purpose: free unblocked 2D array
    Inputs: unblocked 2D array to be freed
    Returns: none
    Expectation: does not access out of bounds cells 

*/
static void a2free(A2 * array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/*
    Function: width
    Purpose: return width
    Inputs: unblocked 2D array 
    Returns: width of 2D array
    Expectation: does not access out of bounds cells 

*/
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/*
    Function: height
    Purpose: return height
    Inputs: unblocked 2D array 
    Returns: height of 2D array
    Expectation: does not access out of bounds cells 

*/
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/*
    Function: size
    Purpose: return size
    Inputs: unblocked 2D array 
    Returns: size of 2D array
    Expectation: does not access out of bounds cells 

*/
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/*
    Function: blocksize
    Purpose: return blocksize
    Inputs: unblocked 2D array 
    Returns: blocksize of 2D array, which is always 1
    Expectation: does not access out of bounds cells 

*/
static int blocksize(A2 array2)
{
        return 1;
  (void) array2;
}


/*
    Function: at
    Purpose: return pointer to a specific index
    Inputs: unblocked 2D array, row, col 
    Returns: pointer to a specific index
    Expectation: does not access out of bounds cells 

*/
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

/*function definition for apply function used for mapping*/
typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

/*
    Function: map_row_major
    Purpose: traverse row-major wise
    Inputs: unblocked 2D array
    Returns: none
    Expectation: does not access out of bounds cells 

*/
static void map_row_major(A2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/*
    Function: map_col_major
    Purpose: traverse col-major wise
    Inputs: unblocked 2D array
    Returns: blocksize of 2D array
    Expectation: does not access out of bounds cells 

*/
static void map_col_major(A2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
  A2Methods_smallapplyfun *apply; 
  void                    *cl;
};


/*function definition for small apply function used for mapping*/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
  struct small_closure *cl = vcl;
  (void)i;
  (void)j;
  (void)uarray2;
  cl->apply(elem, cl->cl);
}


/*
    Function: small_map_row_major
    Purpose: traverse small_map row_major wise
    Inputs: unblocked 2D array, apply function, closure
    Returns: N/A
    Expectation: does not access out of bounds cells 

*/
static void small_map_row_major(A2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_row_major(a2, apply_small, &mycl);
}

/*
    Function: small_map_col_major
    Purpose: traverse small_map col_major wise
    Inputs: unblocked 2D array, apply function, closure
    Returns: N/A
    Expectation: does not access out of bounds cells 

*/
static void small_map_col_major(A2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
  new,
  new_with_blocksize,
  a2free,
        width,
        height,
        size,
        blocksize,                   // blocksize
        at,
        map_row_major,
        map_col_major,
        NULL,                   // map_block_major
        map_row_major,         // map_default
        small_map_row_major,
        small_map_col_major,
        NULL,                 //small_map_block_major
        small_map_row_major,    // small_map_default
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
