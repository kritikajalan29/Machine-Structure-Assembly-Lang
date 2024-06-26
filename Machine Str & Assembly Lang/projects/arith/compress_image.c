//*****************************************************************************
//
//                          compress_image.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: The program takes in a ppm image and compresses it. The 
//              compressed image is represented as a sequence of 32- bit code 
//              words. The program implements all necessary steps including 
//              chroma quantization, discrete cosine transform and conversion 
//              from rgb pixels to component video pixels to fully compress the
//              image.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

#include "compress_image.h"

struct cv {
        double Y, pb, pr;
};

struct quant {
        unsigned int avg_pb, avg_pr, a;
        double y1, y2, y3, y4;
        signed int b, c, d;
};

struct closure {
        A2Methods_UArray2 arr;
        A2Methods_T methods;
};

/*
    Function: compress
    Purpose: calls all necessary functions to compress a ppm image
    Inputs: Pnm_ppm original - the original ppm image to be converted 
                                as a Pnm_ppm type containing the height,
                                width, denominator and array of pixels
    Returns: N/A
    Expectation: the Pnm_ppm image should be a valid ppm image read in by the
                 Pnm_ppmread function
*/
void compress(Pnm_ppm original, A2Methods_T methods)
{
        assert(methods);

        original = trim(original);

        /* retrieves image width, height, and rgb pixel array */
        unsigned int width = original->width;
        unsigned int height = original->height;
        A2Methods_UArray2 rgb_pixels = original->pixels;

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
        Pnm_ppmfree(&original);
        methods->free(&blocks);
        methods->free(&words_array);
}

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
        if (width % 2 != 0) {
                width = width - 1;
        }
        if (height % 2 != 0) {
                height = height - 1;
        }

        Pnm_ppm new_image;
        NEW(new_image);
        new_image->width = width;
        new_image->height = height;
        new_image->denominator = original->denominator;
        new_image->methods = original->methods;
        
        UArray2_T temp = original->methods->new(width, height, 
                                                sizeof(struct Pnm_rgb));
        new_image->pixels = temp;

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
    Function: RGB_to_CV
    Purpose: An apply function called on the pixel array in the Pnm_ppm struct 
    that converts each rgb pixel into floating point representation & then into
    Y/PB / PR (composite video) format using the given formula
    Inputs: int col → column of current index
            int row → row of current index
            UArray2 pixels → array of pixels that stores Pnm_rgb struct 
                             instances as values 
            void *val → Pnm_rgb struct at current index
            void *cl → closure struct → contains UArray2 of cv_pixels and 
                                        an instance of A2Methods
    Returns: N/A
    Expectation: original cannot be null //??
*/
void RGB_to_CV(int col, int row, A2Methods_UArray2 pixels, void *val, void *cl)
{
        closure cls = (closure) cl;
        A2Methods_T methods = cls->methods;

        Pnm_rgb rgb_pixels = (Pnm_rgb) val;
        double r = (double)(rgb_pixels->red);
        double g = (double)(rgb_pixels->green);
        double b = (double)(rgb_pixels->blue);  

        r = r / 255.0;
        g = g / 255.0;
        b = b / 255.0;
        
        double  y, pb, pr; 
        
        y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        pb = - (0.168736 * r) - (0.331264 * g) + (0.5 * b);
        pr = (0.5 * r) - (0.418688 * g) - (0.081312 * b);

        pb = bound_check(pb, 0.5);
        pr = bound_check(pr, 0.5);
        
        cv cv_temp = methods->at(cls->arr, col, row);
        cv_temp->Y = y;
        cv_temp->pb = pb;
        cv_temp->pr = pr;

        (void) pixels;
}

/*
    Function: chroma_quantization
    Purpose: An apply function called on the pixel array in the Pnm_ppm struct 
    that converts each rgb pixel into floating point representation & then into
    Y/PB / PR (composite video) format using the given formula
    Inputs: Pnm_ppm original - the original image as read in from a given file
    Returns: N/A
    Expectation:  //??
*/
void chroma_quantization(int width, int height, A2Methods_UArray2 cv_pixels, 
                         A2Methods_UArray2 quant_pixels, A2Methods_T methods)
{
        double avg_pb, avg_pr;
        unsigned int bit_rep_pb, bit_rep_pr;

        for (int r = 0; r < height; r += 2) {
                for (int c = 0; c < width; c += 2) {
                        cv curr = (cv) methods->at(cv_pixels, c, r);
                        cv cell2 = (cv) methods->at(cv_pixels, c + 1, r);
                        cv cell3 = (cv) methods->at(cv_pixels, c, r + 1);
                        cv cell4 = (cv) methods->at(cv_pixels, c + 1, r + 1);

                        quant block = (quant) methods->at(quant_pixels, c / 2, 
                                                          r / 2);
                
                        block->y1 = curr->Y;
                        block->y2 = cell2->Y;
                        block->y3 = cell3->Y;
                        block->y4 = cell4->Y;

                        avg_pb = (curr->pb + cell2->pb + cell3->pb + cell4->pb);
                        avg_pb = avg_pb / 4.0;

                        avg_pr = (curr->pr + cell2->pr + cell3->pr + cell4->pr);
                        avg_pr = avg_pr / 4.0;

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
    Expectation: 
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

        a = (y4 + y3 + y2 + y1) / 4.0;
        b = (y4 + y3 - y2 - y1) / 4.0;
        c = (y4 - y3 + y2 - y1) / 4.0;
        d = (y4 - y3 - y2 + y1) / 4.0;

        int final_a = scaling(a, 511);
        int final_b = scaling(b, 50);
        int final_c = scaling(c, 50);
        int final_d = scaling(d, 50);
        
        block->a = final_a;
        block->b = final_b;
        block->c = final_c;
        block->d = final_d;

        (void) col;
        (void) row;
        (void) q_blocks;
        (void) cl;
}

/*
    Function: bit_pack
    Purpose: Calls bit_pack_word which packs the information of one quant struct
             into one corresponding 32-bit-code-word. This function creates 
             and returns an array of the packed code words. 
    Inputs: struct closure → contains the UArray of quant blocks and an
                             instance of methods
    Returns: UArray2 words_array → array of 32 bit code-words
    Expectation: 
*/
void *bit_pack(closure cls)
{
        uint64_t code_word;
        A2Methods_T methods = cls->methods;

        int height = methods->height(cls->arr);
        int width = methods->width(cls->arr);
        A2Methods_UArray2 words_array = methods->new_with_blocksize(width, 
                                                 height, sizeof(code_word), 1);

        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        code_word = bit_pack_word(methods->at(cls->arr, col, 
                                                              row));
                        uint64_t *word_address = methods->at(words_array, col, 
                                                             row);
                        *word_address = code_word;
                }
        }

        return words_array;
}

/*
    Function: bit_pack_word
    Purpose: Packs a, b, c, d, avg_PB, and avg_PR into 32-bit words using the 
    Bitpack_newu( ) function. 32-bit words are stored as integers 
    Inputs: quant block → contains a, b, c, d, avg_pb, avg_pr values for one
                          2 x 2 block
    Returns: uint64_t code_word → a packed 32-bit-code-word
    Expectation: 
*/
uint64_t bit_pack_word(quant block)
{
        unsigned int code_word = 0; 
        
        unsigned int a = block->a;
        signed int b = block->b;
        signed int c = block->c;
        signed int d = block->d;
        
        unsigned int pb = block->avg_pb;
        unsigned int pr = block->avg_pr;

        code_word = Bitpack_newu(code_word, 9, 23, a);

        code_word = Bitpack_news(code_word, 5, 18, b);
        code_word = Bitpack_news(code_word, 5, 13, c);
        code_word = Bitpack_news(code_word, 5, 8, d);
        
        code_word = Bitpack_newu(code_word, 4, 4, pb);
        code_word = Bitpack_newu(code_word, 4, 0, pr);
        
        return code_word;
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

        width = width / 2; 
        height = height / 2;

        for (uint row = 0; row < height; row++) {
                for (uint col = 0; col < width; col++) {

                        uint64_t *codeword = methods->at(words_array, col, row);
                        uint64_t word = *codeword;

                        for (int i = 24; i >= 0; i = i - 8) {
                                uint64_t temp = Bitpack_getu(word, 8, i);
                                putchar(temp);
                        }
                }
        }
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
        if (factor == 50) {
                /* bound b, c, and d between -0.3 and 0.3 */
                to_scale = bound_check(to_scale, 0.3);
        }

        return_val = (int) round(to_scale * factor);

        return return_val;
}

/*
    Function: bound_check
    Purpose: Checks whether given value is in bounds 
    Inputs: double value → value to be checked
            double bound → value to be checked against
    Returns: double value → the value with updated bounds
    Expectation: 
*/
double bound_check(double value, double bound)
{
        if (value > bound) {
                value = bound;
        }
        else if (value < (-1.0 * bound)) {
                value = (-1.0 * bound);
        }

        return value;
}
