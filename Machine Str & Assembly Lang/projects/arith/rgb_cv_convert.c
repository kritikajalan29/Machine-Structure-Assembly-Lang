//*****************************************************************************
//
//                          rgb_cv_convert.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: This module converts rgb pixels to component video pixels and 
//              component video pixels back to rgb pixels.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

#include "rgb_cv_convert.h"

const double BR_BOUND = 0.5;

/* Compression Functions */

/*
    Function: RGB_to_CV
    Purpose: An apply function called on the pixel array in the Pnm_ppm struct 
    that converts each rgb pixel into floating point representation & then into
    Y/PB/PR (composite video) format using the given formula
    Inputs: int col → column of current index
            int row → row of current index
            UArray2 pixels → array of pixels that stores Pnm_rgb struct 
                             instances as values 
            void *val → Pnm_rgb struct at current index
            void *cl → closure struct → contains UArray2 of cv_pixels and 
                                        an instance of A2Methods
    Returns: N/A
    Expectation: N/A
*/
void RGB_to_CV(int col, int row, A2Methods_UArray2 pixels, void *val, void *cl)
{
        closure cls = (closure) cl;
        A2Methods_T methods = cls->methods;

        /* accesses rgb values from pixels array */
        Pnm_rgb rgb_pixels = (Pnm_rgb) val;
        double r = (double)(rgb_pixels->red);
        double g = (double)(rgb_pixels->green);
        double b = (double)(rgb_pixels->blue);  

        /* scales rgb values */
        r = r / DENOMINATOR;
        g = g / DENOMINATOR;
        b = b / DENOMINATOR;
        
        double  y, pb, pr; 

        /* computes y, pb, and pr vals from rgb */
        y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        pb = - (0.168736 * r) - (0.331264 * g) + (0.5 * b);
        pr = (0.5 * r) - (0.418688 * g) - (0.081312 * b);

        /* uses helper function to ensure pb and pr are within range */
        pb = bound_check(pb, BR_BOUND);
        pr = bound_check(pr, BR_BOUND);
        
        /* populates cv_pixels array */
        cv cv_temp = methods->at(cls->arr, col, row);
        cv_temp->Y = y;
        cv_temp->pb = pb;
        cv_temp->pr = pr;

        (void) pixels;
}

/* Deompression Functions */

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

        /* accesses CompVideo values from pixels array */
        double y = cv_pixels->Y;
        double pb = cv_pixels->pb;
        double pr = cv_pixels->pr;

        float r, g, b;

        /* computes rgb vals from y, pb, and pr */
        r = DENOMINATOR * (1.0 * y + 0.0 * pb + 1.402 * pr);
        g = DENOMINATOR * (1.0 * y - 0.344136 * pb - 0.714136 * pr);
        b = DENOMINATOR * (1.0 * y + 1.772 * pb + 0.0 * pr);

        /* uses helper function to ensure rgb are within range */
        r = bound_check(r, DENOMINATOR);
        g = bound_check(g, DENOMINATOR);
        b = bound_check(b, DENOMINATOR);

        /* populates cv_pixels array */
        Pnm_rgb rgb_pixels = methods->at(cls->arr, col, row);
        rgb_pixels->red = r;
        rgb_pixels->green = g;
        rgb_pixels->blue = b;

        (void) pixels;

}
