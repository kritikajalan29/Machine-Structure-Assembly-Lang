//*****************************************************************************
//
//                          quantization.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: This module is concerned with quantization. It quantizes the 
//              chroma values of each pixel to average values for a pixel block.
//              It also use discrete cosine transform to transform the 
//              luminance values. The values are then scaled and used to
//              further carry out the compression algorithm. This module
//              also deals with inverse chroma quantization and inverse dct
//              for images to be decompressed.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

#include "quantization.h"

const int A_SCALE = 63; // scale factor : (2^6) - 1
const int BCD_SCALE = 50;
const double BCD_BOUND = 0.3;
const int BLOCKSIZE = 4;

/* Compression Functions */

/*
    Function: chroma_quantization
    Purpose: An apply function called on the pixel array in the Pnm_ppm struct 
    that converts each rgb pixel into floating point representation & then into
    Y/PB / PR (composite video) format using the given formula
    Inputs: Pnm_ppm original - the original image as read in from a given file
    Returns: N/A
    Expectation: width and height have to be >= 0, cv_pixels, quant_pixels,
                 and methods have to be non-null.
*/
void chroma_quantization(int width, int height, A2Methods_UArray2 cv_pixels, 
                         A2Methods_UArray2 quant_pixels, A2Methods_T methods)
{
        assert(width >= 0 && height >= 0);
        assert(methods);

        double avg_pb, avg_pr;
        unsigned int bit_rep_pb, bit_rep_pr;

        /* populates blocks with information from CompVideo pixels */
        for (int r = 0; r < height; r += 2) {
                for (int c = 0; c < width; c += 2) {
                        /* gets access to the 4 pixels in a block */
                        cv curr = (cv) methods->at(cv_pixels, c, r);
                        cv cell2 = (cv) methods->at(cv_pixels, c + 1, r);
                        cv cell3 = (cv) methods->at(cv_pixels, c, r + 1);
                        cv cell4 = (cv) methods->at(cv_pixels, c + 1, r + 1);

                        quant block = (quant) methods->at(quant_pixels, c / 2, 
                                                          r / 2);
                        
                        /* populates blocks after computing and quantizing */
                        block->y1 = curr->Y;
                        block->y2 = cell2->Y;
                        block->y3 = cell3->Y;
                        block->y4 = cell4->Y;

                        avg_pb = (curr->pb + cell2->pb + cell3->pb + 
                                  cell4->pb) / BLOCKSIZE;

                        avg_pr = (curr->pr + cell2->pr + cell3->pr + 
                                  cell4->pr) / BLOCKSIZE;

                        bit_rep_pb = Arith40_index_of_chroma(avg_pb);
                        bit_rep_pr = Arith40_index_of_chroma(avg_pr);
                        
                        block->avg_pb = bit_rep_pb;
                        block->avg_pr = bit_rep_pr;
                }
        }
        
}

/*
    Function: dct
    Purpose: An apply function called on the UArray2 that stores the quant 
    structs (which represent 2x2 blocks of quantized pixels). The function 
    computes a, b, c, d from 4 values of Y in 2x2 blocks. Call scaling functions
    to scale a, b, c, d and convert them into bit representation.
    Inputs: int col → column of current index 
            int row → row of current index
            UArray2 quant → array of 2x2 blocks of pixels that stores quant 
            struct instances as values
            void *val → quant struct at current index
            void *cl → closure argument (NULL)
    Returns: N/A
    Expectation: N/A
*/
void dct (int col, int row, A2Methods_UArray2 q_blocks, void *val, void *cl)
{       
        double y1, y2, y3, y4;
        double a, b, c, d;

        quant block = (quant) val;

        y1 = block->y1;
        y2 = block->y2;
        y3 = block->y3;
        y4 = block->y4;

        /* computes a, b, c, and d from block pixels' luminance  */
        a = (y4 + y3 + y2 + y1) / BLOCKSIZE;
        b = (y4 + y3 - y2 - y1) / BLOCKSIZE;
        c = (y4 - y3 + y2 - y1) / BLOCKSIZE;
        d = (y4 - y3 - y2 + y1) / BLOCKSIZE;

        /* scales a, b, c, and d and populates block */
        int final_a = scaling(a, A_SCALE);
        int final_b = scaling(b, BCD_SCALE);
        int final_c = scaling(c, BCD_SCALE);
        int final_d = scaling(d, BCD_SCALE);
        
        block->a = final_a;
        block->b = final_b;
        block->c = final_c;
        block->d = final_d;

        (void) col;
        (void) row;
        (void) q_blocks;
        (void) cl;
}

/* Decompression Functions */

/*
    Function: inverse_chroma_quantization
    Purpose: Computes the floating point representations of the 
             avg_pb and avg_pr values from the respective blocks using
             the Arith40_chroma_of_index() function. Populates the CompVideo
             pixel structs in the cv_pixels array.
    Inputs: A2Methods_UArray2 q_blocks - array of quant blocks from which the
                                         bit representation of avg_pb and 
                                         avg_pr is retrieved.
            A2Methods_UArray2 cv_pixels - CompVideo pixels array that the 
                                          function populates
            A2Methods_T methods - the methods struct the program uses
    Returns: N/A
    Expectation: q_blocks, cv_pixels, and methods cannot be null
*/
void inverse_chroma_quantization(A2Methods_UArray2 q_blocks, 
                                 A2Methods_UArray2 cv_pixels, 
                                 A2Methods_T methods)
{
        assert(q_blocks);
        assert(cv_pixels);
        assert(methods);

        int width = methods->width(q_blocks);
        int height = methods->height(q_blocks);
        
        double avg_pb, avg_pr;

        /* retrives values from blocks and populates the CompVideo pixels */
        for (int r = 0; r < height; r++) {
                for (int c = 0; c < width; c++) {
                        quant curr = methods->at(q_blocks, c, r);

                        avg_pb = Arith40_chroma_of_index(curr->avg_pb);
                        avg_pr = Arith40_chroma_of_index(curr->avg_pr);

                        /* accesses all the pixels in a block */
                        cv cell1 = methods->at(cv_pixels, (c * 2),(r * 2));
                        cv cell2 = methods->at(cv_pixels, (c * 2) + 1, (r * 2));
                        cv cell3 = methods->at(cv_pixels, (c * 2), (r * 2) + 1);
                        cv cell4 = methods->at(cv_pixels, (c * 2) + 1, 
                                                                (r * 2) + 1);

                        cell1->pb = avg_pb;
                        cell1->pr = avg_pr;

                        cell2->pb = avg_pb;
                        cell2->pr = avg_pr;

                        cell3->pb = avg_pb;
                        cell3->pr = avg_pr;

                        cell4->pb = avg_pb;
                        cell4->pr = avg_pr;
                }
        
        }

}

/*
    Function: inverse_dct
    Purpose: An apply function called on the q_blocks array that retrieves the
             luminance values for each pixel in a 2x2 block
    Inputs: int col - column of the current block
            int row - row of the current block
            A2Methods_UArray2 q_blocks - block array that the map function is 
                                         called on
            void *val - the current quant block
            void *cl - a closure struct that contains the CompVideo pixel array 
                       and the A2Methods_T instance used in the program.
    Returns: N/A
    Expectation: N/A
 */
void inverse_dct(int col, int row, A2Methods_UArray2 q_blocks, void *val, 
                                                                void *cl)
{
        closure cls = (closure) cl;
        A2Methods_T methods = cls->methods;
        
        double a, b, c, d;
        double y1, y2, y3, y4;

        quant block = val;
        
        /* descales a, b, c, and d */
        a = (double) (block->a) / A_SCALE;
        b = (double) (block->b) / BCD_SCALE;
        c = (double) (block->c) / BCD_SCALE;
        d = (double) (block->d) / BCD_SCALE;
        
        /* computes block pixels' luminance */
        y1 = a - b - c + d;
        y2 = a - b + c - d;
        y3 = a + b - c - d;
        y4 = a + b + c + d;

        block->y1 = y1;
        block->y2 = y2;
        block->y3 = y3;
        block->y4 = y4;

        /* accesses all the pixels in a block */
        cv cell1 = methods->at(cls->arr, (col * 2), row * 2);
        cv cell2 = methods->at(cls->arr, (col * 2) + 1, row * 2);
        cv cell3 = methods->at(cls->arr, col * 2, (row * 2) + 1);
        cv cell4 = methods->at(cls->arr, (col * 2) + 1, (row * 2) + 1);

        /* populates block pixels' luminance */
        cell1->Y = y1;
        cell2->Y = y2;
        cell3->Y = y3;
        cell4->Y = y4;

        (void) q_blocks;
}

/*
    Function: scaling
    Purpose: Helper function to scale the a, b, c, d values  
    Inputs: double to_scale → the value to be scaled in floating-point 
                              representation
            int factor → the factor by which a value must be scaled 
    Returns: int return_val → the scaled integer
    Expectation: 
*/
int scaling(double to_scale, int factor)
{
        int return_val;

        /* check if scaling b, c, or d */
        if (factor == BCD_SCALE) {
                /* bound b, c, and d between -0.3 and 0.3 */
                to_scale = bound_check(to_scale, BCD_BOUND);
        }

        return_val = (int) round(to_scale * factor);

        return return_val;
}


