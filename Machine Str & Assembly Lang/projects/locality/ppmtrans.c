//****************************************************************
//
//                          ppmtrans.c
//
//
//     Author: Kritika Jalan and Arya Prasad
//     
//     Purpose: Rotates a ppm image by the specified degree 
//              (0/90/180/270), using a specified method
//              (row/column/block major). Makes use of 
//              A2Methods interface to access implementations
//              of two dimensional plain and blocked UArrays
//
//     Tufts University, Spring 2022
//     CS 40: HW3 locality
//
//     Date: 10/7/2022
//
//****************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mem.h"
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

/*define functions based on method provided by user*/
#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

/* Used as closure for mapping
 * Contains image diemensions, method, array of pixels
 */
struct toTransform{
        A2Methods_T methods;
        A2Methods_UArray2 transform_array;
        int width;
        int height;
        
};

/*function defintions*/
void rotate_0(Pnm_ppm og_image);
void info_90(Pnm_ppm og_image, struct toTransform *cl);
void rotate_90(int col, int row, A2Methods_UArray2 array2D, 
                 void *value, void *cl);
void info_180(Pnm_ppm og_image, struct toTransform *cl);
void rotate_180(int col, int row, A2Methods_UArray2 array2D, 
                 void *value, void *cl);
void info_270(Pnm_ppm og_image, struct toTransform *cl);
void rotate_270(int col, int row, A2Methods_UArray2 array2D, 
                 void *value, void *cl);
void record_time(char *time_file, int rotation, double total_time, 
        Pnm_ppm final_image);

/*Raises an error when client uses program incorrectly*/
static void usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}


/*
    Function: main()
    Purpose: Read in image
             Considers client specifications about the image transformation
                - Use A2Methods to define specific functions based on the 
                  method the client wants
                - Default method: row-major
             Transform the image accordingly by using the functions defined
             by the A2Methods interface and its subclasses
                - - Default degree of rotation: 0
             Update new image information and write to file
             Calculate time it took to transform image if client specifies
    Inputs: argc - number of elements on the command line
            argv - array holding the elements on the command line
    Returns: returns 0 if successful else 1
    Expectation: Usage should be accurate 

*/
int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;
        FILE *fileptr;
        (void) time_file_name;
        (void) rotation;
        (void) i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else{
                        break;
                }
                        

        }


        /*read either from file or stdin*/
        if(i != argc) {
                fileptr = fopen(argv[i], "rd");
        } else {
                fileptr = stdin;
        }
        
        if (fileptr == NULL) {
                fprintf(stderr, "Unable to read file\n");
                exit(EXIT_FAILURE);
        }

        /*use pnm.h to read file and create image*/
        Pnm_ppm og_image, transf_image;
        og_image = Pnm_ppmread(fileptr, methods);
        
        if(i != argc) {
                fclose(fileptr);
        }
        
        /*initialize timer*/
        CPUTime_T track_time;
        track_time = CPUTime_New();
        double end_time;
        (void) end_time;

        /*no transformation required for 0*/
        if (rotation == 0) {
                CPUTime_Start(track_time);
                rotate_0(og_image);
                end_time =CPUTime_Stop(track_time);

                record_time(time_file_name, rotation, end_time, og_image);

                CPUTime_Free(&track_time);
                Pnm_ppmfree(&og_image);
                exit(EXIT_SUCCESS);
        }

        /*initalize final image*/
        NEW0(transf_image);
        
        /*store the method and final image (it's pixels stored in array) 
        in a struct to use as closure during transformation*/
        struct toTransform *cl = malloc(sizeof(*cl));
        cl->methods = methods; 


        if (rotation == 90) {  
                /*update width and height*/   
                info_90(og_image, cl);

                /*transform image*/
                CPUTime_Start(track_time);
                map(og_image->pixels, rotate_90, cl);
                end_time =CPUTime_Stop(track_time);
                /*print to time file*/
                record_time(time_file_name, rotation, end_time, og_image);

        } else if (rotation == 180) {
                info_180(og_image, cl);
                CPUTime_Start(track_time);
                map(og_image->pixels, rotate_180, cl);
                end_time =CPUTime_Stop(track_time);
                record_time(time_file_name, rotation, end_time, og_image);

        } else if (rotation == 270) {
                info_270(og_image, cl);
                CPUTime_Start(track_time);
                map(og_image->pixels, rotate_270, cl);
                end_time =CPUTime_Stop(track_time);
                record_time(time_file_name, rotation, end_time, og_image);
        }


       
        /*update final image information in Pnm_ppm struct*/
        transf_image->denominator = og_image->denominator;
        transf_image->methods = methods;
        transf_image->pixels = cl->transform_array;
        transf_image->width = cl->width;
        transf_image->height = cl->height;

        /*write final image after transformation*/
        Pnm_ppmwrite(stdout, transf_image);

        /*free allocated memory*/
        CPUTime_Free(&track_time);
        Pnm_ppmfree(&og_image);
        Pnm_ppmfree(&transf_image);
        free(cl);


        exit(EXIT_SUCCESS);
        
}

void rotate_0(Pnm_ppm og_image)
{
        Pnm_ppmwrite(stdout, og_image);
}

/*
    Function: info_90
    Purpose: Updates the parameters such as height and width for the
             image to be transformed correctly. It also allocates 
             memory for the 2D UArray that will hold the image rasters.
    Inputs: Pnm_ppm og_image - source image which is to be transformed
            toTransfer cl - struct that will hold transformed image and 
            holds method of transformation
    Returns: none
    Expectation: og_image should not be empty, values inside closure should
                        not be empty

*/
void info_90(Pnm_ppm og_image, struct toTransform *cl)
{
        cl->width = cl->methods->height(og_image->pixels);
        cl->height = cl->methods->width(og_image->pixels);

        /*allocate space for transformed image where the pixel size is
                of type struct Pnm_rgb*/
        
        cl->transform_array = cl->methods->new(cl->width, cl->height, 
                sizeof(struct Pnm_rgb)); 

}

/*
    Function: rotate_90
    Purpose: Apply function to rotate the image by 90 degrees
    Inputs: int col - column index of the 2D UArray where the
            apply function must be used 
            int row - row index of the 2D UArray where the
            apply function must be used
            A2Methods_UArray2 array2D - Array that contains
            pixels of original image to be transformed
            *value - pointer to the value held in the given
            index 
            *cl - pointe to the struct that will hold transformed 
            image and holds method of transformation
    Returns: none
    Expectation: does not access out of bounds cells 

*/
void rotate_90(int col, int row, A2Methods_UArray2 array2D, 
                void *value, void *cl)
{
        struct toTransform *local_struct = cl;
        A2Methods_T methods = local_struct->methods;
        A2Methods_UArray2 tarray = local_struct->transform_array;
        int height = methods->height(array2D);
        
       *((Pnm_rgb)methods->at(tarray, (height - row - 1), col)) = 
        *((Pnm_rgb)value);

}


/*
    Function: info_180
    Purpose: Updates the parameters such as height and width for the
             image to be transformed correctly . It also allocates 
             memory for the 2D UArray that will hold the image rasters.
    Inputs: Pnm_ppm og_image - source image which is to be transformed
            toTransfer cl - struct that will hold transformed image and 
            holds method of transformation
    Returns: none
    Expectation: og_image should not be empty, values inside closure should
                        not be empty

*/
void info_180(Pnm_ppm og_image, struct toTransform *cl)
{
        cl->width = cl->methods->width(og_image->pixels);
        cl->height = cl->methods->height(og_image->pixels);

        /*allocate space for transformed image where the pixel size is
                of type struct Pnm_rgb*/
        cl->transform_array = cl->methods->new(cl->width, cl->height, 
                sizeof(struct Pnm_rgb)); 
}

/*
    Function: rotate_180
    Purpose: Apply function to rotate the image by 180 degrees
    Inputs: int col - column index of the 2D UArray where the
            apply function must be used 
            int row - row index of the 2D UArray where the
            apply function must be used
            A2Methods_UArray2 array2D - Array that contains
            pixels of original image to be transformed
            *value - pointer to the value held in the given
            index 
            *cl - pointe to the struct that will hold transformed 
            image and holds method of transformation
    Returns: none
    Expectation: does not access out of bounds cells 

*/
void rotate_180(int col, int row, A2Methods_UArray2 array2D, 
                void *value, void *cl)
{
        struct toTransform *local_struct = cl;
        A2Methods_T methods = local_struct->methods;
        A2Methods_UArray2 tarray = local_struct->transform_array;
        int height = methods->height(array2D);
        int width = methods->width(array2D);
        
       *((Pnm_rgb)methods->at(tarray,  (width - col - 1), 
                (height - row - 1))) = *((Pnm_rgb)value);


} 

/*
    Function: info_270
    Purpose: Updates the parameters such as height and width for the
             image to be transformed correctly . It also allocates 
             memory for the 2D UArray that will hold the image rasters.
    Inputs: Pnm_ppm og_image - source image which is to be transformed
            toTransfer cl - struct that will hold transformed image and 
            holds method of transformation
    Returns: none
    Expectation: og_image should not be empty, values inside closure should
                        not be empty

*/
void info_270(Pnm_ppm og_image, struct toTransform *cl)
{
        cl->width = cl->methods->height(og_image->pixels);
        cl->height = cl->methods->width(og_image->pixels);

        /*allocate space for transformed image where the pixel size is
                of type struct Pnm_rgb*/
        cl->transform_array = cl->methods->new(cl->width, cl->height, 
                sizeof(struct Pnm_rgb)); 
}

/*
    Function: rotate_270
    Purpose: Apply function to rotate the image by 180 degrees
    Inputs: int col - column index of the 2D UArray where the
            apply function must be used 
            int row - row index of the 2D UArray where the
            apply function must be used
            A2Methods_UArray2 array2D - Array that contains
            pixels of original image to be transformed
            *value - pointer to the value held in the given
            index 
            *cl - pointe to the struct that will hold transformed 
            image and holds method of transformation
    Returns: none
    Expectation: does not access out of bounds cells 

*/
void rotate_270(int col, int row, A2Methods_UArray2 array2D, 
                void *value, void *cl)
{
        struct toTransform *local_struct = cl;
        A2Methods_T methods = local_struct->methods;
        A2Methods_UArray2 tarray = local_struct->transform_array;
        int width = methods->width(array2D);
        
       *((Pnm_rgb)methods->at(tarray,  row, (width - col - 1))) = 
        *((Pnm_rgb)value);

}



/*
    Function: record_time
    Purpose: Write recorded time it took to transform image
    Inputs: file name that holds time info, degree of rotation, total time
            it took to transform image, array of pixels after transformation,
            method of transformation
    Returns: none
    Expectation: file name is provided, file is opened and ready to write
                 final_image is correctly transformed
*/
void record_time(char *time_file, int rotation, double total_time, 
        Pnm_ppm final_image)
{
        if (time_file == NULL) {
                return;
        }
        
        FILE *fp;
        fp = fopen(time_file, "a");
        assert(fp != NULL);
        double time_per_pixel;

        fprintf(fp, "Total time for a %d rotation: %f\n", 
                rotation, total_time);
        time_per_pixel = total_time / 
                (final_image->width * final_image->height);
        fprintf(fp, "Time per pixel for a %d rotation: %f\n", 
                rotation, time_per_pixel);

        fclose(fp);
        
}