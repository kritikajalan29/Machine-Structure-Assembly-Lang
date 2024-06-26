#include <uarray2b.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include "mem.h"
#include "math.h"


void change_value (int col, int row, UArray2b_T array2b, 
                                void *elem, void *cl);
void check_and_print(int i, int j, UArray2b_T a, void *p1, void *p2);

 int main()
{
        UArray2b_T test_array;
        test_array = UArray2b_new(10, 10, 100, 4); // width = 4, height = 4, blocksoze = 4
       
        printf("Width: %d\n", UArray2b_width(test_array));
        printf("Height: %d\n", UArray2b_height(test_array));
        printf("Blocksize: %d\n",UArray2b_blocksize(test_array));
        printf("Size: %d\n", UArray2b_size(test_array));
        // printf("Width of array: %d\n", UArray2b_width_arr(test_array));
        // printf("Height of array: %d\n", UArray2b_height_arr(test_array));


        int value = 1;
        for (int i= 0; i < 10; i++) {
                for(int j = 0; j < 10; j++) {
                      *(int*)(UArray2b_at(test_array, i, j)) = value;
                      value++ ;
                }

        }
        for (int i= 0; i < 10; i++) {
                for(int j = 0; j < 10; j++) {
                      printf("%d " , *(int*)(UArray2b_at(test_array, i, j)));
                }
                printf("\n");

        }

        UArray2b_map(test_array, check_and_print, 00);

        printf("New Array: \n");

        for (int i= 0; i < 10; i++) {
                for(int j = 0; j < 10; j++) {
                      printf("%d " , *(int*)(UArray2b_at(test_array, i, j)));
                }
                printf("\n");

        }


        printf("Elem at index (9,9) is: %d\n", *(int*)(UArray2b_at(test_array, 9, 9)));

        UArray2b_free(&test_array);
        return 0;
}

void change_value (int col, int row, UArray2b_T array2b, 
                                void *elem, void *cl)
{
        *(int*)elem += 1;
        (void) col;
        (void) row;
        (void) cl;
        (void) array2b;
}

void
check_and_print(int i, int j, UArray2b_T a, void *p1, void *p2) 
{
    (void)p2;
    (void)i;
    (void)j;
    (void)a;
    *(int*)p1 += 1;
}