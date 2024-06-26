#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include "mem.h"

#include "bitpack.h"
#include "seq.h"
#include "operations.h"

int main()
{
        Seq_T segments = Seq_new(4);
        uint32_t *instruction0;
        instruction0 = calloc(5, sizeof(uint32_t));
        Seq_addhi(segments, instruction0);

        uint32_t *instruction1;
        instruction1 = calloc(5, sizeof(uint32_t));
        Seq_addhi(segments, instruction1);

        uint32_t *instruction2;
        instruction2 = calloc(5, sizeof(uint32_t));
        for(int i = 0; i < 5; i++) {
                instruction2[i] = i;
        }
        Seq_addhi(segments, instruction2);
        
        test_segload();
        seg_store(1, 3, 5, segments);

        
        seg_load(7, 1, 3, segments);
        print_segload();

        return 0;


}