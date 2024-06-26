#include <string.h>

#include <a2blocked.h>
#include "uarray2b.h"

// define a private version of each function in A2Methods_T that we implement

typedef A2Methods_UArray2 A2;	// private abbreviation

/*
    Function: new
    Purpose: Apply function to rotate the image by 90 degrees
    Inputs: int width, height, size - dimensions and info
            about image
    Returns: blocked 2D array 
    Expectation: does not access out of bounds cells 

*/
static A2 new(int width, int height, int size)
{
	return UArray2b_new_64K_block(width, height, size);
}

/*
    Function: new_with_blocksize
    Purpose: Create blocked 2D array with blocksize
    Inputs: int width, height, size, blocksize - dimensions and info
            about image
    Returns: blocked 2D array 
    Expectation: does not access out of bounds cells 

*/
static A2 new_with_blocksize(int width, int height, int size, int blocksize)
{
	return UArray2b_new(width, height, size, blocksize);
}

/*
    Function: a2free
    Purpose: free blocked 2D array
    Inputs: blocked 2D array to be freed
    Returns: none
    Expectation: does not access out of bounds cells 

*/
static void a2free(A2 * array2p)
{
	UArray2b_free((UArray2b_T *) array2p);
}

/*
    Function: width
    Purpose: return width
    Inputs: blocked 2D array
    Returns: width of 2D array
    Expectation: does not access out of bounds cells 

*/
static int width(A2 array2)
{
	return UArray2b_width(array2);
}

/*
    Function: height
    Purpose: return height
    Inputs: blocked 2D array
    Returns: height of 2D array
    Expectation: does not access out of bounds cells 

*/
static int height(A2 array2)
{
	return UArray2b_height(array2);
}

/*
    Function: size
    Purpose: return size
    Inputs: blocked 2D array 
    Returns: size of 2D array
    Expectation: does not access out of bounds cells 

*/
static int size(A2 array2)
{
	return UArray2b_size(array2);
}

/*
    Function: blocksize
    Purpose: return blocksize
    Inputs: blocked 2D array 
    Returns: blocksize of 2D array
    Expectation: does not access out of bounds cells 

*/
static int blocksize(A2 array2)
{
	return UArray2b_blocksize(array2);
}

/*
    Function: at
    Purpose: return pointer to a specific index
    Inputs: blocked 2D array, row, col
    Returns: pointer to a specific index
    Expectation: does not access out of bounds cells 

*/
static A2Methods_Object *at(A2 array2, int i, int j)
{
	return UArray2b_at(array2, i, j);
}

/*function definition for apply function used for mapping*/
typedef void applyfun(int i, int j, UArray2b_T array2b, void *elem, void *cl);

static void map_block_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
	UArray2b_map(array2, (applyfun *) apply, cl);
}

struct small_closure {
	A2Methods_smallapplyfun *apply;
	void *cl;
};

/*function definition for small apply function used for mapping*/
static void apply_small(int i, int j, UArray2b_T array2, void *elem, void *vcl)
{
	struct small_closure *cl = vcl;
	(void)i;
	(void)j;
	(void)array2;
	cl->apply(elem, cl->cl);
}

/*
    Function: small_map_block_major
    Purpose: traverse small_map block_major wise
    Inputs: unblocked 2D array, apply function, closure
    Returns: N/A
    Expectation: does not access out of bounds cells 

*/
static void small_map_block_major(A2 a2, A2Methods_smallapplyfun apply,
				  void *cl)
{
	struct small_closure mycl = { apply, cl };
	UArray2b_map(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_blocked_struct = {
	new,
	new_with_blocksize,
	a2free,
	width,
	height,
	size,
	blocksize,
	at,
	NULL,			// map_row_major
	NULL,			// map_col_major
	map_block_major,
	map_block_major,	// map_default
	NULL,			// small_map_row_major
	NULL,			// small_map_col_major
	small_map_block_major,
	small_map_block_major,	// small_map_default
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_blocked = &uarray2_methods_blocked_struct;
