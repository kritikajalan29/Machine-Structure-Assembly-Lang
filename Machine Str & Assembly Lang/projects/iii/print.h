/*
 * print.h
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * print.h is the interface for a helper module used by unblackedges. 
 * It declares all the functions to print the pbm file created following 
 * processing done by unblackedges
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "bit2.h"

#ifndef PRINT_INCLUDED
#define PRINT_INCLUDED

extern void print_pbm(int height, int width, Bit2_T raster);


#endif