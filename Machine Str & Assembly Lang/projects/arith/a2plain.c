/*
 * a2plain.c
 * locality
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 13th, 2022
 *
 * A method suite for the UArray2_T implementation. It holds pointers to
 * all the UArray2_T implementation functions so that clients can access 
 * the functions indirectly without knowing details of the implementation.
 *
 */

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
    Purpose: Calls UArray2_new through the A2Methods method suite
             to create a UArray2_T with given dimensions and size.
    Inputs: int width - given width used to initialise UArray2_T
            int height - given height used to initialise UArray2_T
            int size - given elem size used to initialise UArray2_T
    Returns: A2Methods_UArray2 instance
    Expectation: width and height have to be >= 0.
                 size has to be > 0
*/
static A2 new(int width, int height, int size)
{ 
        return UArray2_new(width, height, size);
}

/*
    Function: new_with_blocksize
    Purpose: Calls UArray2_new through the A2Methods method suite
             to create a UArray2_T with given dimensions and size.
    Inputs: int width - given width used to initialise UArray2_T
            int height - given height used to initialise UArray2_T
            int size - given elem size used to initialise UArray2_T
            int blocksize - ignored because no blocksize in UArray2_T
    Returns: A2Methods_UArray2 instance
    Expectation: width and height have to be >= 0.
                 size has to be > 0
                 blocksize is ignored
*/
static A2 new_with_blocksize(int width, int height, int size, 
                                            int blocksize)
{
        (void) blocksize;

        return UArray2_new(width, height, size);
}

/*
    Function: a2free
    Purpose: Calls UArray2_free through the A2Methods method suite
             to deallocate memory used for uarray2
    Inputs: A2 *array2p - Pointer to an A2Methods_UArray2 object
    Returns: N/A
    Expectation: array2p and *array2p have to be non-null
*/
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/*
    Function: width
    Purpose: Calls UArray2_width through the A2Methods method suite
             to return width of given uarray2
    Inputs: A2 array2 - an instance of A2Methods_UArray2 object
    Returns: width of given uarray
    Expectation: array2 parameter has to be non-null

*/
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/*
    Function: height
    Purpose: Calls UArray2_height through the A2Methods method suite
             to return height of given uarray2
    Inputs: A2 array2 - an instance of A2Methods_UArray2 object
    Returns: height of given uarray
    Expectation: array2 parameter has to be non-null

*/
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/*
    Function: size
    Purpose: Calls UArray2_size through the A2Methods method suite
             to return size of given uarray2
    Inputs: A2 array2 - an instance of A2Methods_UArray2 object
    Returns: size of given uarray
    Expectation: array2 parameter has to be non-null

*/
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/*
    Function: blocksize
    Purpose: Returns 0 becuase UArray2_T objects do not have blocksize
    Inputs: A2 array2 - an instance of A2Methods_UArray2 object
    Returns: 0 (no blocks in a UArray2_T)
    Expectation: array2 parameter has to be non-null

*/
static int blocksize(A2 array2)
{
        (void) array2;
        return 0;
}

/*
    Function: at
    Purpose: Calls UArray2_at through the A2Methods method suite
             to return a pointer to uarray element at index (i, j)
    Inputs: int i - column number of an index
            int j - row number of an index
            A2 array2 - an instance of A2Methods_UArray2 object
    Returns: a pointer to an A2Methods_UArray2 object
    Expectation: i has to be between 0 and one less than the number of
                 columns in array2; j has to be be between 0 and one 
                 less than the number of rows in the array2; array2 has to be
                 non-null

*/
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

/*
    Function: map_row_major
    Purpose: Calls UArray2_map_row_major through the A2Methods method suite
             to traverse through the uarray2 one row at a time and perform 
             the function provided to each element of the uarray2
    Inputs: A2Methods_UArray2 uarray2 - an instance of A2Methods_UArray2
            an A2Methods_applyfun object (holds apply function)
            void *cl - closure argument
    Returns: N/A  
    Expectation: uarray2 parameter should be non-null

*/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/*
    Function: map_col_major
    Purpose: Calls UArray2_map_col_major through the A2Methods method suite
             to traverse through the uarray2 one column at a time and perform
             the function provided to each element of the uarray2
    Inputs: A2Methods_UArray2 uarray2 - an instance of A2Methods_UArray2
            an A2Methods_applyfun object (holds apply function)
            void *cl - closure argument
    Returns: N/A  
    Expectation: uarray2 parameter should be non-null

*/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/*
    Function: apply_small
    Purpose: 
    Inputs: int i - column number of current elem
            int j - row number of current elem
            UArray2_T uarray2 - uarray2 that is being traversed
            void *elem - pointer to the current elem 
            void *vcl - closure argument
    Returns: N/A  
    Expectation: i and j should be within the range of uarray2; uarray2 has
                 has to be non-null.

*/
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
    Purpose: Calls UArray2_map_row_major through the A2Methods method suite
             to traverse through the uarray2 one row at a time and perform 
             the function provided to each element of the uarray2. It calls
             it with the 'apply_small' apply function and 'mycl' argument.
    Inputs:  A2Methods_UArray2 a2 - an instance of A2Methods_UArray2
             A2Methods_smallapplyfun apply - an A2Methods_smallapplyfun 
             object (holds small apply function)
             void *cl - closure argument
    Returns: N/A  
    Expectation: uarray2 parameter should be non-null

*/
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/*
    Function: small_map_col_major
    Purpose: Calls UArray2_map_col_major through the A2Methods method suite
             to traverse through the uarray2 one column at a time and perform 
             the function provided to each element of the uarray2. It calls
             it with the 'apply_small' apply function and 'mycl' argument.
    Inputs:  A2Methods_UArray2 a2 - an instance of A2Methods_UArray2
             A2Methods_smallapplyfun apply - an A2Methods_smallapplyfun 
             object (holds small apply function)
             void *cl - closure argument
    Returns: N/A  
    Expectation: uarray2 parameter should be non-null

*/
static void small_map_col_major(A2Methods_UArray2        a2,
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
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL,
        map_row_major,
        small_map_row_major,
        small_map_col_major,
        NULL,
        small_map_row_major,
};

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
