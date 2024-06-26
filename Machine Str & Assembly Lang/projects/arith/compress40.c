//*****************************************************************************
//
//                          compress40.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: The program takes in a file pointer (either a file or stdin) 
//              and based on the command (-c or -d) it either compresses the
//              image file or decompresses it. 
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "pnm.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "rgb_cv_convert.h"
#include "quantization.h"
#include "file_io.h"
#include "codeword.h"


/*
    Function: compress40
    Purpose: calls upon the compress_image module to compress a PPM image
    Inputs: FILE *input - the file that contains a ppm image to be compressed
    Returns: N/A
    Expectation: the input file should be open and contain a valid ppm image 
                 read in by the Pnm_ppmread function.
*/
void compress40(FILE *input) 
{
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods);

        Pnm_ppm original;
        original = Pnm_ppmread(input, methods);

        Pnm_ppm updated = trim(original);

        /* retrieves image width, height, and rgb pixel array */
        unsigned int width = updated->width;
        unsigned int height = updated->height;
        A2Methods_UArray2 rgb_pixels = updated->pixels;

        A2Methods_UArray2 cv_pixels = methods->new(width, height, 
                                                   sizeof(struct cv));
        A2Methods_UArray2 blocks = methods->new(width / 2, height / 2, 
                                                sizeof(struct quant));

        /* declares closure struct to pass in a cv_pixels & methods as a 
                                                        closure argument */
        closure cls1;
        NEW(cls1);
        cls1->arr = cv_pixels;
        cls1->methods = methods;

        /* convert RGB pixels to CompVideo pixels */        
        methods->map_default(rgb_pixels, RGB_to_CV, cls1);
        
        /* quantize chroma values and populate blocks array */
        chroma_quantization(width, height, cv_pixels, blocks, methods);

        /* conduct the discrete cosine transform */
        methods->map_default(blocks, dct, NULL);

        /* declares closure struct to pass in a blocks & methods as a 
                                                        closure argument */
        closure cls2;
        NEW(cls2);
        cls2->arr = blocks;
        cls2->methods = methods;
        A2Methods_UArray2 words_array = bit_pack(cls2);

        /* writes compressed image to stdout */
        
        write_compressed(words_array, methods);

        /* frees memory */
        FREE(cls1);
        FREE(cls2);
        methods->free(&cv_pixels);
        methods->free(&blocks);
        methods->free(&words_array);
        Pnm_ppmfree(&original);
        Pnm_ppmfree(&updated);
}

/*
    Function: decompress40
    Purpose: calls upon the decompress_image module to decompress a compressed 
             image and write it as a PPM image
    Inputs: FILE *input - the file that contains a compressed image to be 
                          decompressed and written as a PPM
    Returns: N/A
    Expectation: the input file should be open and contain a valid compressed 
                 image that can be processed by the decompress_image module.
*/
void decompress40(FILE *input) 
{
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods);

        Pnm_ppm image = read_header(input);

        unsigned int width = image->width;
        unsigned int height = image->height;

        A2Methods_UArray2 rgb_pixels = methods->new(width, height, 
                                                sizeof(struct Pnm_rgb));
        A2Methods_UArray2 cv_pixels = methods->new(width, height, 
                                                        sizeof(struct cv));
        
        image->denominator = 255;
        image->pixels = rgb_pixels;
        image->methods = methods;

        closure cls;
        NEW(cls);
        cls->arr = cv_pixels;
        cls->methods = methods;

        A2Methods_UArray2 codewords = codeword_reader(width / 2, height / 2, 
                                                                input, methods);

        A2Methods_UArray2 blocks = (A2Methods_UArray2) bit_unpack(codewords, 
                                                                methods);

        methods->map_default(blocks, inverse_dct, cls);

        inverse_chroma_quantization(blocks, cv_pixels, methods);


        cls->arr = rgb_pixels;

        methods->map_default(cv_pixels, CV_to_RGB, cls);

        write_decompressed(image);

        FREE(image);
        FREE(cls);
        methods->free(&blocks);
        methods->free(&cv_pixels);
        methods->free(&codewords);
        methods->free(&rgb_pixels);
}