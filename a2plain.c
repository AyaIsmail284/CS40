/*
 *  a2plain.c
 *  Bayan Tuffaha btuffa01, Aya Ismail aismai02
 *  2/22/2024
 *
 *  CS 40 HW 3 Locality
 *
 *  Summary: Implementation of the A2Methods_UArray2 interface used in 
 *           the polymorphic A2Methods_T interface, suited to perform
 *           operations on UArray2s
 *           
 *      
 */

#include <string.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/


/********************** new *******************
 * creates a UArray2 or UArray2b with the given dimensions 
 *  
 * inputs: width, height, size
 *
 * return: the created A2Methods_UArray2 
 * 
 *
 *********************************************************/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/********************** new_with_blocksize *******************
 * creates a UArray2 or UArray2b with the given dimensions and blocksizw
 *  
 * inputs: width, height, size, blocksize
 *
 * return: the created A2Methods_UArray2 
 * 
 * notes: blocksize of a plain UArray is 1
 *********************************************************/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/********************** a2free *******************
 * frees the allocated memory of the given UArray2 or UArray2b represented 
 * by the polymorphic type A2Methods_UArray2 
 *  
 * inputs: A2Methods_UArray2 array2p
 *
 * return: none
 * 
 * side effects: calls UArray2_free which frees the allocated space for the 
 * given UArray2
 *
 *********************************************************/
static void a2free(A2Methods_UArray2 * array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/********************** width *******************
 * returns the width of the given UArray2 or UArray2b 
 *  
 * inputs: A2Methods_UArray2 array2p
 *
 * 
 * side effects: calls UArray2_width which frees the allocated space for the 
 * given UArray2
 *
 *********************************************************/
static int width(A2Methods_UArray2 array2)
{
        return UArray2_width(array2);
}

/********************** height *******************
 * returns the height of the given UArray2 or UArray2b 
 *  
 * inputs: A2Methods_UArray2 array2p
 *
 *********************************************************/
static int height(A2Methods_UArray2 array2)
{
        return UArray2_height(array2);
}

/********************** size *******************
 * returns the size of the given UArray2 or UArray2b 
 *  
 * inputs: A2Methods_UArray2 array2p
 *
 *********************************************************/
static int size(A2Methods_UArray2 array2)
{
        return UArray2_size(array2);
}

/********************** blocksize *******************
 * returns the blocksize of the given UArray2 or UArray2b 
 *  
 * inputs: A2Methods_UArray2 array2p
 *
 *********************************************************/
static int blocksize(A2Methods_UArray2 array2)
{
        (void) array2;
        return 1;
}

/********************** at ******************************
 * returns the element at the given i, j indices of the given 
 * UArray2 or UArray2b 
 *  
 * inputs: pointer to the element of type A2Methods_object
 *
 *********************************************************/
static A2Methods_Object *at(A2Methods_UArray2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2_T array2b, void *elem,
                              void *cl);

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,       
        at,
        map_row_major,                   
        map_col_major,            
        NULL,                   // map_block_major
        map_col_major,          // map_default 
        small_map_row_major,    
        small_map_col_major,    
        NULL,                   // small_map_block_major
        small_map_col_major     // small_map_default 
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
