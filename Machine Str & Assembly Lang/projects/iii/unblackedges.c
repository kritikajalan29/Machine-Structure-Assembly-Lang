/*
 * unblackedges.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * unblackedges.c uses the bit2 interface to remove black edges from a 
 * scanned image. The image is stored in a 2D bitmap and with the help of DFS
 * helper module, it scans through all black edge pixels and their neighbouring
 * black pixels and turns them white.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "pnmrdr.h"
#include "bit2.h"
#include "seq.h"
#include "print.h"
#include "DFS.h"

/* Function declarations */
static FILE *open_file(char *fname, char *mode);
static void pbm_reader(FILE *fp);

/*
    Function: main
    Purpose: deals with file I/O and calls pbm_reader
    Inputs: int argc, char *argv[]
               argc is the number of arguments in the command line
               and argv holds the names of the commands
    Returns: 0
    Expectation: argc should be less than equal to 2.
    Notes: raise CRE if argc > 2. raises CRE if file not closed

*/
int main(int argc, char *argv[])
{
        assert(argc <= 2);

        /* input from stdin */
        if (argc == 1) {
                pbm_reader(stdin);
        }
        /* Input from file */
        else if (argc == 2) {
                FILE *file = open_file(argv[1], "r");
                pbm_reader(file);
                int check_close = fclose(file);
                assert(check_close == 0);
        }
        return 0;
}

/*
    Function: open_file
    Purpose: opens file of given name in specified mode
    Inputs: a pointer to fname and mode.
               fname is the name of the file and mode is the mode of opening 
               (reading, writing etc.)
    Returns: Pointer to a filestream object pointing to open file/stdin
    Expectation: filestream fp cannot be null
    Notes: raises CRE if filestream fp is null

*/
static FILE *open_file(char *fname, char *mode) 
{
        FILE *fp = fopen(fname, mode);
        assert(fp != NULL);
        return fp;
}

/*
    Function: pbm_reader
    Purpose: uses the pnmrdr reader to check if supplied file is a pbm image in
                the correct format. Main driver code that calls all helper
                functions to unblack black edge pixels and neighbours
    Inputs: a pointer to a filestream object which points to the open
               file
    Returns: N/A
    Expectation: filestream has to contain a pbm
    Notes: raises CRE if filestream does not have a pbm. raises CRE if
           heigh and width are 0.

*/
static void pbm_reader(FILE *fp)
{
        /* reads in pbm header and checks for correct format */
        Pnmrdr_T reader = Pnmrdr_new(fp);       
        Pnmrdr_mapdata data = Pnmrdr_data(reader);
        assert(data.type == 1);
        int height = data.height;
        int width = data.width;
        assert(height != 0 && width != 0);
        
        /* Populates Bit2_T instance with image raster */
        Bit2_T raster = Bit2_new(width, height);
        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                        Bit2_put(raster, j, i, Pnmrdr_get(reader));
                } 
        }

        /* calls helper function to conduct DFS and unblacks black edge
           and neighbouring black pixels */
        Bit2_map_row_major(raster, DFS, NULL);
        print_pbm(height, width, raster);

        /* frees dynamically allocated memory */
        Pnmrdr_free(&reader);
        Bit2_free(&raster);
        
}
