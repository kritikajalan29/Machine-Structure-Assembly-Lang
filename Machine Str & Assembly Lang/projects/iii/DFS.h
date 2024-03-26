/*
 * DFS.h
 * iii
 *
 * Authors: Ishaan Arora (iarora01) and Kritika Jalan (kjalan01)
 * Last Edited: October 4th, 2022
 *
 * DFS.h is the interface for a helper module for unblackedges.c. It declares
 * all the functions required to run a depth first search through the black
 * edge pixels and their neighbours in a pbm and turns them white.
 *
 */

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "bit2.h"
#include "seq.h"

#ifndef DFS_INCLUDED
#define DFS_INCLUDED

#define T Bit_Coordinates
typedef struct T *T;

extern void DFS(int col, int row, Bit2_T raster, int val, void *cl);
extern void neighbour_search(Bit2_T raster, int col, int row, Seq_T *stack);
extern bool in_bounds(Bit2_T bitmap, int col, int row);
extern void *populate_bit_coord (int col, int row);

#endif