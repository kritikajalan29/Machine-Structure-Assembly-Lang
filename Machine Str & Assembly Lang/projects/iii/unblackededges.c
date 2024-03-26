#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "pnmrdr.h"
#include "bit2.h"

static FILE *open_file(char *fname, char *mode);
static void pbm_reader(FILE *fp);

int main(int argc, char *argv[])
{
        assert(argc <= 2);
        /* Test stdin */
        if (argc == 1) {
                pbm_reader(stdin);
        }
        else if (argc == 2) {
                FILE *file = open_file(argv[1], "r"); /* open given file */
                pbm_reader(file);
                int check_close = fclose(file); /* close file */
                assert(check_close == 0);
        }
        return 0;
}
static FILE *open_file(char *fname, char *mode) 
{
        FILE *fp = fopen(fname, mode);
        assert(fp != NULL);
        return fp;
}
static void pbm_reader(FILE *fp)
{
        Pnmrdr_T reader = Pnmrdr_new(fp);       
        Pnmrdr_mapdata data = Pnmrdr_data(reader);
        assert(data.type == 1);
        int height = data.height;
        int width = data.width;
        int maxval = data.denominator;
        assert(height != 0 && width != 0)
        
}