/*
 *  uarray2.c
 *  Aya Ismail aismai02, Bayan Tuffaha btuffa01
 *  2/12/2024
 *
 *  CS 40 HW 2 iii
 *
 *  Summary: implementation of the UArray2_T ADT interface
 *           (based on Hanson's UArray_T ADT)
 *      
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uarray.h"
#include "uarray2.h"
#include "mem.h"
#include "except.h"
#include "assert.h"

/* 2D Unboxed Array Representation */
struct UArray2_T {
        int width; 
        int height; 
        int size; 
        UArray_T outer;
};

/********************** UArray2_new **********************
 * creates a 2D unboxed array with the given dimensions
 * 
 * inputs: int width, int height, int size (number of bytes) 
 *         of the elements to store
 *
 * return: UArray2_T (pointer to UArray2) with these dimensions
 *
 * expects: width, height, and size are positive
 *
 * side effects: the memory allocated for the 2d array must
 *               be freed later on
 *********************************************************/
UArray2_T UArray2_new(int width, int height, int size)
{
        assert((width >= 0) && (height >= 0) && (size > 0)); 

        UArray_T outer = UArray_new(width, sizeof(UArray_T));

        for (int i = 0; i < width; i++) {
                UArray_T inner = UArray_new(height, size);
                *((UArray_T *)UArray_at(outer, i)) = inner;
        }

        UArray2_T arr;
        NEW(arr);
        arr->width = width;
        arr->height = height;
        arr->size = size;
        arr->outer = outer;

        return arr;
}

/********************** UArray2_width ********************
 * takes in a UArray2 and returns its width (number of cols)
 * 
 * inputs: UArray2_T (pointer to a UArray2)
 *
 * return: int width of the UArray2
 *
 * expects: UArray2_T not NULL, returned width is positive
 *********************************************************/
int UArray2_width(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->width;
}

/********************** UArray2_height *******************
 * takes in a UArray2 and returns its height (number of rows)
 * 
 * inputs: UArray2_T (pointer to a UArray2)
 *
 * return: int height of the UArray2
 *
 * expects: UArray2_T not NULL, returned height is positive
 *********************************************************/
int UArray2_height(UArray2_T uarray2)
{       
        assert(uarray2 != NULL);
        return uarray2->height;
}

/********************** UArray2_size ********************
 * takes in a UArray2 and returns the size of the elements
 * it stores 
 * 
 * inputs: UArray2_T (pointer to a UArray2)
 *
 * return: int size of elements the array stores
 *
 * expects: UArray2_T not NULL, returned size is positive
 *********************************************************/
int UArray2_size(UArray2_T uarray2) 
{       
        assert(uarray2 != NULL);
        return uarray2->size; 
}

/*********************** UArray2_at *********************
 * retrieves the element of the given UArray2 at the given
 * col and row indices, or sets an element at those indices
 * 
 * inputs: UArray2_T (pointer to a UArray2), int column
 *         index, int row index
 *
 * return: pointer to the element stored at those indices
 *
 * expects: UArray2_T not NULL, returned pointer is not NULL,
 *          indices are within bounds
 *********************************************************/
void *UArray2_at(UArray2_T uarray2, int col, int row)
{
        assert(uarray2 != NULL);
        assert((col >= 0) && (col < uarray2->width) && (row >= 0) && 
               (row < uarray2->height));

        UArray_T *inner = UArray_at(uarray2->outer, col);
        void *elem = UArray_at(*inner, row);
        assert(elem != NULL);

        return elem;
}

/****************** UArray2_map_col_major ****************
 * calls the given apply function for each elem of the given
 * UArray2, iterating through the 2d array column by column
 * 
 * inputs: UArray2_T (pointer to a UArray2), apply function,
 *         closure (void pointer)
 *
 * return: none
 *
 * expects: UArray2_T not NULL;
 *          apply function is of type void and takes in the col
 *          and row indices, void pointer to element stored at
 *          those indices, and a closure (void pointer);
 *          closure state may be modified with each call to apply
 *********************************************************/
void UArray2_map_col_major(UArray2_T uarray2, 
                           void apply(int col, int row, UArray2_T uarray2,
                           void *p1, void *p2), void *cl)
{
        assert(uarray2 != NULL);

        for (int col = 0; col < uarray2->width; col++) {
                for (int row = 0; row < uarray2->height; row++) {
                        apply(col, row, uarray2, 
                              UArray2_at(uarray2, col, row), cl);
                }
        }        
}

/****************** UArray2_map_row_major ****************
 * calls the given apply function for each elem of the given
 * UArray2, iterating through the 2d array row by row
 * 
 * inputs: UArray2_T (pointer to a UArray2), apply function,
 *         closure (void pointer)
 *
 * return: none
 *
 * expects: UArray2_T not NULL;
 *          apply function is of type void and takes in the col
 *          and row indices, void pointer to element stored at
 *          those indices, and a closure (void pointer);
 *          closure state may be modified with each call to apply
 *********************************************************/
void UArray2_map_row_major(UArray2_T uarray2, void apply(int col, int row,
                           UArray2_T uarray2, void *p1, void *p2), void *cl)
{
        assert(uarray2 != NULL);

        for (int row = 0; row < uarray2->height; row++) {
                for (int col = 0; col < uarray2->width; col++) {
                        apply(col, row, uarray2, 
                              UArray2_at(uarray2, col, row), cl);
                }
        }          
}

/*********************** UArray2_free *********************
 * deallocates and clears the 2D unboxed array
 * 
 * inputs: pointer to UArray2_T (pointer to pointer to
 *         a UArray2)
 *
 * return: none
 *
 * expects: UArray2_T and *UArray2_T not NULL
 *********************************************************/
void UArray2_free(UArray2_T *uarray2)
{
        assert((uarray2 != NULL) && (*uarray2 != NULL));
        
        for (int i = 0; i < (*uarray2)->width; i++) {
                UArray_free(UArray_at((*uarray2)->outer, i));
        }

        UArray_T outer = (*uarray2)->outer;
        UArray_free(&outer);
        FREE(*uarray2);
}