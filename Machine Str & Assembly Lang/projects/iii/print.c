/*
 * print.c
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * print.c is a helper module used by unblackedges. It defines all the 
 * functions to print the pbm file created following processing done 
 * by unblackedges.
 *
 */
 
 #include "print.h"

void print_pbm(int height, int width, Bit2_T Bit2)
{
    
    printf("P1\n"); /* magic number of a pbm = P1 */
    printf("%d %d\n", width, height);
    for (int i = 0; i < height; i++) {
        // char *line = Seq_get(*sequence, i);
        for (int j = 0; j < width; j++) {
            // printf("%c",line[i]);
            int value = Bit2_get(Bit2, j, i);
            printf("%d", value);
        }
        printf("\n");
    }
}