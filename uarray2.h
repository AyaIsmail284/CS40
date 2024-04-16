/*
 *  uarray2.h
 *  Aya Ismail aismai02, Bayan Tuffaha btuffa01
 *  2/12/2024
 *
 *  CS 40 HW 2 iii
 *
 *  Summary: interface of the UArray2_T ADT, representing a 2d
 *           version of Hanson's UArray_T ADT
 *      
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uarray.h"

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

typedef struct UArray2 *UArray2_T;


UArray2_T UArray2_new(int width, int height, int size); 

int UArray2_width(UArray2_T uarray2); 

int UArray2_height(UArray2_T uarray2); 

int UArray2_size(UArray2_T uarray2);

void *UArray2_at(UArray2_T uarray2, int col, int row); 

void UArray2_map_col_major(UArray2_T uarray2, void apply(int col, int row, 
                           UArray2_T uarray2, void *p1, void *p2), void *cl); 

void UArray2_map_row_major(UArray2_T uarray2, void apply(int col, int row,
                           UArray2_T uarray2, void *p1, void *p2), void *cl); 

void UArray2_free(UArray2_T *uarray2); 

#undef UArray2_T
#endif