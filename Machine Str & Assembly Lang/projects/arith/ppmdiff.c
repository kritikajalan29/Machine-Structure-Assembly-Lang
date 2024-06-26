#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "pnm.h"
#include "uarray2.h"
#include "uarray.h"
#include "mem.h"
#include "a2methods.h"
#include "a2plain.h"

double diff(int width, int height, Pnm_ppm image1, Pnm_ppm image2);

int main(int argc, char *argv[]) 
{
        

        FILE *fp1, *fp2;
        if (strcmp(argv[1], "-") == 0) {
                fp1 = stdin;
        } else {
                fp1 = fopen(argv[1], "r");
        }
        
        if (strcmp(argv[2], "-") == 0) {
                fp2 = stdin;
        } else {
                fp2 = fopen(argv[2], "r");
        }
        assert (fp1 != NULL && fp2 != NULL);

        A2Methods_T methods = uarray2_methods_plain;

        Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
        Pnm_ppm image2 = Pnm_ppmread(fp2, methods);

        int width, height;

        int width1 = image1->width;
        int height1 = image1->height;
        int width2 = image2->width;
        int height2 = image2->height;

        if ((width1 - width2) == 0 || (width1 - width2) == 1 ) {
                width = width2;
        } else if ((width2 - width1) == 0 || (width2 - width1) == 1 ) {
                width = width1;
        }
        else {
                fprintf(stderr, 
                "File width are different by a margin greater than 1.\n");
                printf("1.0\n");
                exit(EXIT_FAILURE);
        }

        if ((height1 - height2) == 0 || (height1 - height2) == 1 ) {
                height = height2;
        } else if ((height2 - height1) == 0 || (height2 - height1) == 1 ) {
                height = height1;
        }
        else {
                fprintf(stderr, 
                "File height are different by a margin greater than 1.\n");
                printf("1.0\n");
                exit(EXIT_FAILURE);
        }
        
        double difference = diff(width, height, image1, image2);
        printf ("difference = %f\n" , difference);

        Pnm_ppmfree(&image1);
        Pnm_ppmfree(&image2);
        fclose(fp1);
        fclose(fp2);
        
        (void) argc;

}

double diff(int width, int height, Pnm_ppm image1, Pnm_ppm image2) 
{

        double r_diff, g_diff, b_diff;
        double r_sum = 0, g_sum = 0, b_sum = 0;
        Pnm_rgb image1_pixels;
        Pnm_rgb image2_pixels;

        double den1 = (double)image1->denominator;
        double den2 = (double) image2->denominator;
        
        double e = 0;
        for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                        image1_pixels = image1->methods->at(image1->pixels, 
                                                                        i, j);
                        image2_pixels = image2->methods->at(image2->pixels, 
                                                                        i, j);
                        
                        r_diff = (((double)(image1_pixels->red)/ den1) - 
                                        ((double)(image2_pixels->red)/den2));
                        r_diff = r_diff * r_diff;

                        g_diff = ((double)(image1_pixels->green)/den1 - 
                                        (double)(image2_pixels->green)/den2);
                        g_diff = g_diff * g_diff;
                        
                        b_diff = (((double)(image1_pixels->blue)/ den1) - 
                                        ((double)(image2_pixels->blue)/den2));
                        b_diff = b_diff * b_diff;
                        

                        r_sum = r_sum + r_diff;
                        g_sum = g_sum + g_diff;
                        b_sum = b_sum + b_diff;
                }
        }


        e = ((r_sum + g_sum + b_sum) / (3 * width * height));
        e = sqrt(e);
        return e;
        
}