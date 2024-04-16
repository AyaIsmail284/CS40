
/*
 *  uarray2b.c
 *  Bayan Tuffaha btuffa01, Aya Ismail aismai02
 *  2/22/2024
 *
 *  CS 40 HW 3 Locality
 *
 *  Summary: Implementation of the UArray2b_T ADT interface 
 *           (based on our implementation of UArray2_T ADT)
 *           provideing functionalities that allow the client 
 *           to use the UArray2b interface like creating and 
 *           initializing a UArray2b, accessing or modifying
 *           elements at specific indices, and performing block
 *           major mapping.
 *      
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uarray.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "mem.h"
#include "except.h"
#include "assert.h"
#include "math.h"

/* Representation of 2D Unboxed Array organized in blocks
   where the blocks are stored in an outer plain 2d array (UArray2_T)
   and each block is represented by a UArray_T */

struct UArray2b_T {
        int width; 
        int height; 
        int size; 
        int blocksize; 
        UArray2_T outer;
};

/* Maximum RAM memory to default the blocksize */
const int MAX_BLOCK_RAM = 64 * 1024;


int determine_blocksize(int size);
void set_array2b_info(UArray2b_T array2b, int width, int height, int size,
                      int blocksize, UArray2_T outer);


/********************** UArray2b_new **********************
 * creates a 2D unboxed blocked array with the given dimensions and blocksize
 * 
 * inputs: width, height, size (number of bytes) of the elements to 
 *         be stored, blocksize (dimension of the squared block)
 *         
 * return: UArray2b_T (pointer to UArray2b) with the given dimensions
 *
 * expects: width, height, size, and blocksize are positive
 *
 * side effects: the memory allocated for the 2d array must
 *               be freed later on
 * 
 * Notes: we use the given height and width of the 2d array to create 
 *        array2b_width: the number of blocks across 
 *        array2b_height: the number of blocks down
 *        These dimensions are used to initialize the inner UArrays (blocks)
 *********************************************************/
extern UArray2b_T UArray2b_new(int width, int height, int size, int blocksize) 
{
        assert((width >= 0) && (height >= 0) && (size > 0) && (blocksize > 0)); 
        
        int array2b_width = ceil((double)width / blocksize); 
        int array2b_height = ceil((double)height / blocksize); 

        UArray2_T outer = UArray2_new(array2b_width, array2b_height, 
                                      sizeof(UArray_T));

        for (int col = 0; col < array2b_width; col++) {
                for (int row = 0; row < array2b_height; row++) {
                        UArray_T inner = UArray_new(blocksize * blocksize,
                                                    size);
                        *((UArray_T *)UArray2_at(outer, col, row)) = inner;
                }
        }     
        
        UArray2b_T array2b;
        NEW(array2b);
        assert(array2b != NULL);
        set_array2b_info(array2b, width, height, size, blocksize, outer);

        return array2b;
}

/********************** set_array2b_info *******************
 * sets the info of the given array2b with the values passed into the function
 *  
 * inputs: UArray2b_T array2b, width, height, size, blocksize, UArray2_T outer 
 *
 * return: none
 *
 *********************************************************/
void set_array2b_info(UArray2b_T array2b, int width, int height, int size,
                      int blocksize, UArray2_T outer)
{
        array2b->width = width;
        array2b->height = height;
        array2b->size = size;
        array2b->blocksize = blocksize;
        array2b->outer = outer;       
}

/********************** UArray2b_new_64K_block **********************
 * creates a 2D unboxed blocked array with the given dimensions 
 * a default blocksize depending on the size of the element to be stored
 * 
 * inputs: width, height, size (number of bytes) of the elements to 
 *         be stored
 *         
 * return: UArray2b_T (pointer to UArray2b) with the given dimensions and
           default blocksize 
 *
 * expects: width, height, and size are positive
 *
 * side effects: the memory allocated for the 2d array must
 *               be freed later on
 * 
 * Notes: 
 *********************************************************/
extern UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        int blocksize = determine_blocksize(size);
        UArray2b_T arr = UArray2b_new(width, height, size, blocksize); 

        return arr;
}

/********************** determine_blocksize *******************
 * Determines the default blocksize used to initialize the UArray2b based
 * on the size of the elements to be stored
 * 
 * inputs: size of the elements to be stored in the UArray2b
 *
 * return: the largest blocksize that can fit in 64 KB in RAM
 *
 * expects: given size is positive, returned blocksize is positive
 *********************************************************/
int determine_blocksize(int size)
{       
        /* size of element is larger than the maximum RAM memory 
           that a block can fit in */
        if (size > MAX_BLOCK_RAM) {
                return 1;
        } else {
                return sqrt(MAX_BLOCK_RAM / size);
        }
}

/*********************** UArray2b_free *********************
 * deallocates and clears the 2D unboxed blocked array, 
 * frees the inner UArrays used for the blocks
 * 
 * inputs: pointer to UArray2b_T (pointer to pointer to
 *         a UArray2b)
 *
 * return: none
 *
 * expects: UArray2b_T and *UArray2b_T not NULL
 *********************************************************/
extern void UArray2b_free(UArray2b_T *array2b)
{
        assert((array2b != NULL) && (*array2b != NULL));

        int array2b_width = ceil((double)((*array2b)->width) / 
                            ((*array2b)->blocksize)); 
        int array2b_height = ceil((double)((*array2b)->height) / 
                             ((*array2b)->blocksize));

        for (int col = 0; col < array2b_width; col++) {
                for (int row = 0; row < array2b_height; row++) {
                        UArray_free(UArray2_at((*array2b)->outer, col, row));
                }
        }   

        UArray2_T outer = (*array2b)->outer;
        UArray2_free(&outer);
        FREE(*array2b);
}

/********************** UArray2b_width ********************
 * takes in a UArray2b and returns its width (number of cols)
 * 
 * inputs: UArray2b_T (pointer to a UArray2b)
 *
 * return: int width of the UArray2b
 *
 * expects: UArray2b_T not NULL, returned width is positive
 *********************************************************/
extern int UArray2b_width(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/********************** UArray2b_height *******************
 * takes in a UArray2b and returns its height (number of rows)
 * 
 * inputs: UArray2b_T (pointer to a UArray2)
 *
 * return: int height of the UArray2b
 *
 * expects: UArray2b_T not NULL, returned height is positive
 *********************************************************/
extern int UArray2b_height(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/********************** UArray2b_size ********************
 * takes in a UArray2b and returns the size of the elements
 * it stores 
 * 
 * inputs: UArray2b_T (pointer to a UArray2)
 *
 * return: int size of elements the array stores
 *
 * expects: UArray2b_T not NULL, returned size is positive
 *********************************************************/
extern int UArray2b_size(UArray2b_T array2b) 
{
        assert(array2b != NULL);
        return array2b->size;
}

/********************** UArray2_blocksize ********************
 * takes in a UArray2 and returns the blocksize (number of cols 
   or rows in a block)
 * 
 * inputs: UArray2_T (pointer to a UArray2)
 *
 * return: int size of elements the array stores
 *
 * expects: UArray2_T not NULL, returned size is positive
 *********************************************************/
extern int UArray2b_blocksize(UArray2b_T array2b) 
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/*********************** UArray2b_at *********************
 * retrieves the element of the given UArray2b at the given cell index within
 * a block given the col and row indices of that block or sets an 
 * element at those specific indices
 * 
 * inputs: UArray2b_T (pointer to a UArray2b), block column and row indice
 *
 * return: pointer to the element stored at those indices
 *
 * expects: UArray2b_T not NULL, returned pointer is not NULL,
 *          indices are within bounds
 *********************************************************/
extern void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);

        int block_col_index = column / (array2b->blocksize);
        int block_row_index = row / (array2b->blocksize);
        int block_cell_index = array2b->blocksize * 
                               (column % array2b->blocksize) + 
                               (row % array2b->blocksize);

        assert((column >= 0) && (column < array2b->width) && (row >= 0) && 
               (row < array2b->height));
        

        UArray_T *inner = UArray2_at(array2b->outer, block_col_index,
                                     block_row_index);

        void *elem = UArray_at(*inner, block_cell_index);
        assert(elem != NULL);

        return elem;
}

/****************** UArray2b_map ****************
 * calls the given apply function for each elem of the given
 * UArray2b, iterating through the 2d array block by block
 * 
 * inputs: UArray2b_T (pointer to a UArray2b), apply function,
 *         closure (void pointer)
 *
 * return: none
 *
 * expects: UArray2b_T not NULL;
 *          apply function is of type void and takes in the col
 *          and row indices, void pointer to element stored at
 *          those indices, and a closure (void pointer);
 *          closure state may be modified with each call to apply
 *
 * notes:   the mapping function does not go into unused cell indices 
 *          within each block as we map the cell index within a block with 
 *          its corresponding col and row indices within the structure of 
 *          the UArray2b given the given width and height dimenions
 *
 *********************************************************/
extern void UArray2b_map(UArray2b_T array2b, void apply(int col, int row, 
                         UArray2b_T array2b, void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);

        int width = array2b->width;
        int height = array2b->height;
        int blocksize = array2b->blocksize;
        
        int num_blocks_across = ceil((double)(width) / blocksize);
        int num_blocks_down = ceil((double)(height) / blocksize);

        for (int block_col_index = 0; block_col_index < num_blocks_across; 
             block_col_index++) {
                for (int block_row_index = 0; block_row_index < 
                     num_blocks_down; block_row_index++) {
                        for (int cell_index = 0; cell_index < 
                            (blocksize * blocksize); cell_index++) {
                                int row_cell_index = 
                                        (cell_index / blocksize) +
                                        (blocksize * block_row_index); 

                                int col_cell_index =
                                        (cell_index % blocksize) + 
                                        (blocksize * block_col_index); 
                                
                                /* checking if current cell is used */ 
                                if ((col_cell_index < width) && 
                                    (row_cell_index < height)) {
                                        apply(col_cell_index, row_cell_index,
                                              array2b, 
                                              UArray2b_at(array2b, 
                                              col_cell_index, 
                                              row_cell_index), cl);
                                }
                        }
                }
        }  
}