//*****************************************************************************
//
//                          file_io.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: This module deals with all the functions needed for input/output
//              and file handling for compression and decompression of images.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

#include "file_io.h"

/* Compression Functions */

/*
    Function: trim
    Purpose: trims width and/or height if either are odd
    Inputs: Pnm_ppm original - the original ppm image to be trimmed
    Returns: Pnm_ppm - ppm image with even numbered height and width
    Expectation: original cannot be null
*/
Pnm_ppm trim(Pnm_ppm original)
{
        assert(original != NULL);
        
        int width = original->width;
        int height = original->height;

        /* checks if width and height are even & updates appropriately */
        if (width % 2 != 0) {
                width = width - 1;
        }
        if (height % 2 != 0) {
                height = height - 1;
        }

        /* copies old Pnm_ppm into a new one with updated height and width */
        Pnm_ppm new_image;
        NEW(new_image);
        new_image->width = width;
        new_image->height = height;
        new_image->denominator = original->denominator;
        new_image->methods = original->methods;
        
        UArray2_T temp = original->methods->new(width, height, 
                                                sizeof(struct Pnm_rgb));
        new_image->pixels = temp;
        
        /* copies updated image raster into new Pnm_ppm */
        for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                        Pnm_rgb curr = (Pnm_rgb) original->methods
                                        ->at(original->pixels, i, j);
                        Pnm_rgb updated = (Pnm_rgb) new_image->methods
                                        ->at(temp, i, j);

                        *updated = *curr;
                }
        }
        
        
        return new_image;
}

/*
    Function: write_compressed
    Purpose: Prints header of compressed image. Prints sequence of 32-bit words 
    in Big Endian order. 
    Inputs: UArray2 words_array → array of 32 bit code-words
            A2Methods methods → the methods suite
    Returns: N/A
    Expectation: 
*/
void write_compressed(A2Methods_UArray2 words_array, A2Methods_T methods)
{
        unsigned int width = 2 * methods->width(words_array);
        unsigned int height = 2 * methods->height(words_array);

        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        printf("\n");

        /* updates to height and width of word array (half that of image) */
        width = width / 2; 
        height = height / 2;

        /* writes codewords to stdout 1 byte at a time */
        for (unsigned int row = 0; row < height; row++) {
                for (unsigned int col = 0; col < width; col++) {

                        uint64_t *codeword = methods->at(words_array, col, 
                                                         row);
                        uint64_t word = *codeword;

                        for (int i = 24; i >= 0; i = i - 8) {
                                uint64_t temp = Bitpack_getu(word, 8, i);
                                putchar(temp);
                        }
                }
        }
}

/* Decompression Functions */

/*
    Function: read_header
    Purpose: reads the header of the compressed image file and populates a 
             Pnm_ppm struct with the values read in.
    Inputs: FILE *input - input file (compressed image) to be read from
    Returns: Pnm_ppm - Populated Pnm_ppm struct representing the decompressed 
                       image
    Expectation: input file should be opened for reading and non-null
*/
Pnm_ppm read_header(FILE *input)
{
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                                                        &width, &height); 
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');
        
        /* creates new Pnm_ppm struct to represent decompressed image */
        Pnm_ppm decomp_image;
        NEW(decomp_image);
        decomp_image->width = width;
        decomp_image->height = height;

        return decomp_image;
}

/*
    Function: codeword_reader
    Purpose: reads the codewords from the input file and populates a UArray2_T
    Inputs: int width - width of the codeword array (half of the width of the 
                        image)
            int height - height of the codeword array (half of the height of 
                         the image)
            FILE *input - input file (compressed image) to be read from
            A2Methods_T methods - instance of A2Methods_T the user uses
    Returns: A2Methods_UArray2 - returns a populated A2Methods_UArray2
    Expectation: width and height should be >= 0, input and methods 
                 should be non-null.
*/
A2Methods_UArray2 codeword_reader(int width, int height, FILE *input, 
                                                        A2Methods_T methods)
{
        A2Methods_UArray2 word_array = methods->new_with_blocksize(width, 
                                                height, sizeof(uint64_t), 1);
        
        /* reads codewords from input file 1 byte at a time */
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {

                        uint64_t word = 0;
                        int byte = 0;

                        for (int i = 24; i >= 0; i = i - 8) {
                                byte = getc(input);
                                /* checks for codeword size and number */
                                assert(byte != EOF);

                                word = Bitpack_newu(word, 8, i, byte);
                        }
                        /* populates array of words */
                        *(uint64_t *) methods->at(word_array, col, row) = word;
                }
        }
        
        return word_array;
}

/*
    Function: write_decompressed
    Purpose: writes the Pnm_ppm image to stdout
    Inputs: Pnm_ppm image - Pnm_ppm struct of the image
    Returns: N/A
    Expectation: Pnm_ppm image struct should be populated
*/
void write_decompressed(Pnm_ppm image)
{
        Pnm_ppmwrite(stdout, image);
}
