//*****************************************************************************
//
//                          decompress_image.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: The program takes in a compressed image file and returns a
//              decompressed ppm image. The program implements all necessary
//              steps including inverse chroma quantization, inverse discrete
//              cosine transform and conversion from component video pixels to
//              rgb pixels to fully decompress the image.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

#include "decompress_image.h"

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
    Function: decompress
    Purpose: calls all necessary functions to decompress a compressed image
    Inputs: FILE *input - input file that contains a compressed image
    Returns: N/A
    Expectation: input should be opened and be non-null
*/
void decompress(FILE *input)
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

/*
    Function: read_header
    Purpose: reads header of input file to ensure it is of the correct format
    Inputs: FILE *input - input file that contains the compressed file
    Returns: UArray2 of codewords
    Expectation: N/A
*/
Pnm_ppm read_header(FILE *input)
{
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                                                        &width, &height); 
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');
        
        Pnm_ppm decomp_image;
        NEW(decomp_image);
        decomp_image->width = width;
        decomp_image->height = height;

        return decomp_image;
}

/*
    Function: codeword_reader
    Purpose: reads 32-bit codewords from inputfile and populates a UArray2_T
    Inputs: int width - width of codeword reader array (half of image width)
            int height - height of codeword reader array (half of image height)
            A2Methods_T methods - instance of A2Methods_T the program uses
    Returns: UArray2 of codewords
    Expectation: N/A
*/
A2Methods_UArray2 codeword_reader(int width, int height, FILE *input, 
                                                        A2Methods_T methods)
{
        A2Methods_UArray2 word_array = methods->new_with_blocksize(width, 
                                                height, sizeof(uint64_t), 1);


        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {

                        uint64_t word = 0;
                        int byte = 0;

                        for (int i = 24; i >= 0; i = i - 8) {
                                byte = getc(input);
                                assert(byte != EOF);
                                word = Bitpack_newu(word, 8, i, byte);
                        }

                        *(uint64_t *) methods->at(word_array, col, row) = word;
                }
        }
        
        return word_array;
}

/*
    Function: bit_unpack
    Purpose: populates the q_blocks array with populated quant blocks
    Inputs: A2Methods_UArray2 words_array - array of codewords
            A2Methods_T methods - instance of A2Methods_T the program uses
    Returns: UArray2_T of codewords
    Expectation: methods cannot be null
*/
void *bit_unpack(A2Methods_UArray2 words_array, A2Methods_T methods)
{
        assert(methods);

        int height = methods->height(words_array);
        int width = methods->width(words_array);

        A2Methods_UArray2 q_blocks = methods->new(width, height, 
                                                        sizeof(struct quant));
        int code_word;
        
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        code_word = (*(uint32_t *)methods->at(words_array, 
                                                                col, row));
                        quant block = bit_unpack_word(code_word);
                        quant address = methods->at(q_blocks, col, row);
                        *address = *block;
                        FREE(block);

                }
        }

        return q_blocks;
} 

/*
    Function: bit_unpack_word
    Purpose: returns a new quant block after populating it with values unpacked
             from the codeword
    Inputs: uint32_t code_word - 32-bit codeword
    Returns: populated quant block 
    Expectation: N/A
*/
quant bit_unpack_word(uint32_t code_word)
{
        quant block;
        NEW(block);
        
        block->a = Bitpack_getu(code_word, 9, 23);
        block->b = Bitpack_gets(code_word, 5, 18);
        block->c = Bitpack_gets(code_word, 5, 13);
        block->d = Bitpack_gets(code_word, 5, 8);

        block->avg_pb = Bitpack_getu(code_word, 4, 4);
        block->avg_pr = Bitpack_getu(code_word, 4, 0);

        return block;
}

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

        for (int r = 0; r < height; r++) {
                for (int c = 0; c < width; c++) {
                        quant curr = methods->at(q_blocks, c, r);

                        avg_pb = Arith40_chroma_of_index(curr->avg_pb);
                        avg_pr = Arith40_chroma_of_index(curr->avg_pr);

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
        
        a = (double) (block->a) / 511.0;
        b = (double) (block->b) / 50.0;
        c = (double) (block->c) / 50.0;
        d = (double) (block->d) / 50.0;
        
        y1 = a - b - c + d;
        y2 = a - b + c - d;
        y3 = a + b - c - d;
        y4 = a + b + c + d;

        block->y1 = y1;
        block->y2 = y2;
        block->y3 = y3;
        block->y4 = y4;

        cv cell1 = methods->at(cls->arr, (col * 2), row * 2);
        cv cell2 = methods->at(cls->arr, (col * 2) + 1, row * 2);
        cv cell3 = methods->at(cls->arr, col * 2, (row * 2) + 1);
        cv cell4 = methods->at(cls->arr, (col * 2) + 1, (row * 2) + 1);

        cell1->Y = y1;
        cell2->Y = y2;
        cell3->Y = y3;
        cell4->Y = y4;

        (void) q_blocks;
}

/*
    Function: CV_to_RGB
    Purpose: An apply function called on the cv_pixels array that converts each 
    CompVideo pixel into an integer and then an RGB pixel.
    Inputs: int col - column of the current pixel
            int row - row of the current pixel
            A2Methods_UArray2 pixels - pixels array that the map function is 
                                       called on
            void *val - the current CompVideo pixel
            void *cl - a closure struct that contains the RGB pixel array and
                       the A2Methods_T instance used in the program.
    Returns: N/A
    Expectation: N/A
*/
void CV_to_RGB(int col, int row, A2Methods_UArray2 pixels, void *val, void *cl) 
{
        closure cls = (closure) cl;
        A2Methods_T methods = cls->methods;

        cv cv_pixels = (cv) val;

        double y = cv_pixels->Y;
        double pb = cv_pixels->pb;
        double pr = cv_pixels->pr;

        float r, g, b;

        r = 255 * (1.0 * y + 0.0 * pb + 1.402 * pr);
        g = 255 * (1.0 * y - 0.344136 * pb - 0.714136 * pr);
        b = 255 * (1.0 * y + 1.772 * pb + 0.0 * pr);
        
        if (r > 255) r = 255;
        if (r < 0) r = 0;
        if (g > 255) g = 255;
        if (g < 0) g = 0;
        if (b > 255) b = 255;
        if (b < 0) b = 0;

        Pnm_rgb rgb_pixels = methods->at(cls->arr, col, row);
        rgb_pixels->red = r;
        rgb_pixels->green = g;
        rgb_pixels->blue = b;

        (void) pixels;

}

void write_decompressed(Pnm_ppm image)
{
        Pnm_ppmwrite(stdout, image);
}
