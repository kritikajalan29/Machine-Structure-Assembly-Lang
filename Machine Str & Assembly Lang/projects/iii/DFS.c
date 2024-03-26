/*
 * DFS.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * DFS.c is the implementation for the DFS search helper module used in 
 * unblackedges.c. It defines all the functions required to run a 
 * unblacken all the black edge pixels and their neighbors by conducting a 
 * depth first search.
 *
 */

#include "DFS.h"

#define T Bit_Coordinates
struct T {
        int x, y;
};

/*
    Function: DFS
    Inputs: int col - column number of given bit
            int row - row number of given bit
            Bit2_T raster - 2D bit vector which stores the pbm image raster
            int val - Value of given bit
            void *cl - closure argument
    Purpose: conducts the depth first search to find and unblack the 
             black edge pixels and their neighbors. 
    Return: N/A
    Expectation: col, row, val, and Bit2_T raster should be non-null. Val
                 should be either 0 or 1. Col and row should be between 0 and
                 raster's width and height respectively.
    Notes: It is an apply function to be called by Bit2_map_row_major
           Raises CRE if raster is null

*/
void DFS(int col, int row, Bit2_T raster, int val, void *cl)
{  
        assert(raster != NULL);

        /* checks if current bit is on the edge  */
        if (row == 0 || row == (Bit2_height(raster) - 1) || col == 0 || 
                                        col == (Bit2_width(raster) - 1)) {
                Seq_T stack = Seq_new(0);
                /* checks if pixel is black */
                if (val == 1) {
                        /* creates a struct to store pixel coordinates */
                        struct Bit_Coordinates *temp; 
                        temp = populate_bit_coord(col, row);

                        Seq_addhi(stack, temp);    
                        /* checks for all neighbors of structs on stack
                           until stack is empty */ 
                        while (Seq_length(stack) != 0) {
                                temp = Seq_remhi(stack);
                                Bit2_put(raster, temp->x, temp->y, 0);
                                neighbour_search(raster, temp->x, temp->y, 
                                                 &stack);
                                free(temp);
                        
                        }  
                }
                Seq_free(&stack);
        }
        (void) cl;
}

/*
    Function: bit_check
    Inputs: int col - column number of given bit
            int row - row number of given bit
            Bit2_T bitmap - 2D bit vector which stores the pbm image raster
    Purpose: checks if given bit is within bounds and black or white
    Return: true if bit is within bounds and black, false otherwise
    Expectation: col, row, and Bit2_T raster should be non-null. Col and row 
                 should be between 0 and raster's width and height 
                 respectively.
    Notes: will CRE if raster is null.

*/
bool bit_check(Bit2_T bitmap, int col, int row)
{
        assert(bitmap != NULL);

        /* checks if current bit is within bounds */
        if (col >= 0 && col < Bit2_width(bitmap) && row >= 0 && 
            row < Bit2_height(bitmap)) {
                int temp_val = Bit2_get(bitmap, col, row);
                /* checks if current bit is black */
                if (temp_val == 1) {
                        return true;
                }
        }
        return false;
        
}

/*
    Function: populate_bit_coord
    Inputs: int col - column number of given bit
            int row - row number of given bit
    Purpose: creates a new struct to store coordinates of pixels
    Return: pointer to new struct instance
    Expectation: col and row should be non-null. Col and row 
                 should be between 0 and raster's width and height 
                 respectively.
    Notes: Allocates memory on the heap using malloc. Raises CRE if coord is
           null

*/
void *populate_bit_coord (int col, int row) 
{
        /* allocates memory on the heap for Bit_Coordinates struct and
           populates it */
        struct Bit_Coordinates *coord; 
        coord = malloc(sizeof(*coord));
        assert(coord != NULL);
        coord->x = col;
        coord->y = row;
        return coord;
}

/*
    Function: neighbour_search
    Inputs: int col - column number of given bit
            int row - row number of given bit
            Bit2_T raster - 2D bit vector which stores the pbm image raster
            Seq_T *stack - pointer to the struct that stores pixels to be
                           processed.
    Purpose: finds neighbours of black edge pixels and pushes them to the
             stack.
    Return: N/A
    Expectation: col, row, *stack, stack and raster should be non-null. 
                 Col and row should be between 0 and raster's width and height
                 respectively.
    Notes: will CRE if stack, *stack, or raster is null.

*/  
void neighbour_search(Bit2_T raster, int col, int row, Seq_T *stack)
{
        assert(raster != NULL && stack != NULL && *stack != NULL);
        struct Bit_Coordinates *temp;

        /* checks each neighbour to see if it is black and pops it on stack */
        if (bit_check(raster, (col - 1), row)) {
                temp = populate_bit_coord((col - 1), row);
                Seq_addhi(*stack, temp);
        }
        
        if (bit_check(raster, col, (row - 1))) {
                temp = populate_bit_coord(col, (row - 1));
                Seq_addhi(*stack, temp);

        }
        
        if (bit_check(raster, (col + 1), row)) {
                temp = populate_bit_coord((col + 1), row);
                Seq_addhi(*stack, temp);
        }
        
        if (bit_check(raster, col, (row + 1))) {
                temp = populate_bit_coord(col, (row + 1));
                Seq_addhi(*stack, temp);
        }               
}
