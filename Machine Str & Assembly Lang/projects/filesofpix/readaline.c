#include "readaline.h"
#include <stdlib.h>
#include "assert.h"

size_t readaline(FILE *inputfd, char **datapp) {
        assert(inputfd != NULL && datapp!= NULL);
        
        size_t x = 1000;
        *datapp = (char *) malloc(x); /* allocating memory for *datapp*/
    
        size_t i = 0;
        
        while (!feof(inputfd))
        {
                if (i == x) {
                    *datapp = (char *) realloc(*datapp, (2*x));
                    x = 2*x;
                }
                (*datapp)[i] = fgetc(inputfd);/* getting one char at a time 
                                                                from file */
                if (((*datapp)[i] == '\n') || ((*datapp)[i] == EOF))
                {
                        break; /* loop breaks if an \n or EOF char 
                                                            is encountered */
                }
                i++;

        }  

        if (feof(inputfd))
        {
                free(*datapp);
                datapp = NULL;
        }
        
        return i;
        
        
}



