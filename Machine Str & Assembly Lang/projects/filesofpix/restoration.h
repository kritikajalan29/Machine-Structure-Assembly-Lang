#include <stdio.h>
#include "readaline.h"
#include "pnmrdr.h"
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include "seq.h"


static void restore(FILE *fp);
static FILE *open_file(char *fname, char *mode);
static int atom_populator(int num_bytes, char *chars, char **digit_array, 
                          char **nondigit_array);
static void print_pic(Seq_T *sequence);
static void free_table(const void *key, void **value, void *cl);



