/******************************************************************
 *  ppmdiff.c
 *  Aya Ismail(aismai02) and Leisha Bhandari (lbhand01)
 *  2/24/24
 *
 *  CS 40 HW 4: Arith
 *
 *  Implementation for ppmdiff, which ...
 *
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "assert.h"
#include "uarray2.h"
#include "a2plain.h"
#include "a2methods.h"
#include "pnm.h"
#include "math.h"



const int MAX_DIFF_HEIGHT_WIDTH = 1; 

static FILE *open_or_abort(char *fname, char *mode);
Pnm_ppm read_pixmap(char* filename, A2Methods_T methods);
void diff(Pnm_ppm pixmap, Pnm_ppm pixmap_to_compare, A2Methods_T methods);
int determine_height(int height, int height_to_compare); 
int determine_width(int width, int width_to_compare); 
void check_height_width(int height1, int height2, int width1, int width2); 
                        
int main(int argc, char *argv[])
{      

        /* expecting two image files */
        // printf("asserted that args are 3\n"); 
        assert(argc == 3);

        
                A2Methods_T methods = uarray2_methods_plain; 
    
                // printf("About to read from first image\n"); 
                Pnm_ppm pixmap = read_pixmap(argv[1], methods); 
                
                // printf("About to read from second image\n"); 
                Pnm_ppm pixmap_to_compare = read_pixmap(argv[2], methods); 

                diff(pixmap, pixmap_to_compare, methods); 
                

        return EXIT_SUCCESS;
}

/******************** open_or_abort ********************
 * tries to open the given file, raises an exception on error
 * 
 * parameters:
 *     char *fname: name of the file to open
 *     char *mode:  mode of file to open
 *
 * return: the opened file
 *
 * notes:
 *     raises exceptions if the given file cannot be opened
 *******************************************************/
static FILE *open_or_abort(char *filename, char *mode)
{       
       
        FILE *fp = fopen(filename, mode);
        if (fp == NULL) {
                int rc = errno;
                fprintf(stderr,
                        "Could not open file %s with mode %s\n",
                        filename,
                        mode);
                exit(rc);
        }        
        return fp;
}

Pnm_ppm read_pixmap(char* filename, A2Methods_T methods) 
{       
        assert(methods != NULL); 
        FILE *fp = open_or_abort(filename, "rb");
        
        assert(fp != NULL); 
        Pnm_ppm pixmap = Pnm_ppmread(fp, methods);
        assert(pixmap != NULL); 
        
        fclose(fp); 
        return pixmap; 
}

void diff(Pnm_ppm pixmap, Pnm_ppm pixmap_to_compare, A2Methods_T methods) 
{
        
        assert((pixmap != NULL) && (pixmap_to_compare != NULL) && (methods != NULL));

        // int height1 = methods->height(pixmap->pixels);
        int height1 = pixmap->height;
        int width1 = pixmap->width; 
        
        // printf("height of the first image %d\n", height1); 
        // printf("width of the first image %d\n", width1);

        int height2 = pixmap_to_compare->height; 
        int width2 = pixmap_to_compare->width; 
        
        // printf("height of the second image %d\n", height2);
        // printf("width of the second image %d\n", width2);

        // ***** DO WE NEED TO EXIT *******
        check_height_width(height1, height2, width1, width2); 
       
        int min_height = determine_height(height1, height2); 
        int min_width = determine_width(width1, width2); 
        // printf("Min height %d\n", min_height);
        // printf("Min width %d\n", min_width);
        
        float rgb_diff_sum = 0; 
        for (int i = 0; i < min_width; i++){
                for (int j = 0; j < min_height; j++){
                        float sum = 0; 

                       
                        Pnm_rgb pixel1 = (Pnm_rgb)pixmap->methods->at(pixmap->pixels, i, j);
                        Pnm_rgb pixel2 = (Pnm_rgb)pixmap->methods->at(pixmap_to_compare->pixels, i, j);
                        
                        float red1_ratio = (float) pixel1->red / pixmap->denominator;
                        float green1_ratio = (float) pixel1->green / pixmap->denominator;
                        float blue1_ratio = (float) pixel1->blue / pixmap->denominator;
                        // printf("red1_ratio %f\n", red1_ratio);
                        // printf("green1_ratio %f\n", green1_ratio);
                        // printf("blue1_ratio %f\n", blue1_ratio);
                        
                        float red2_ratio = (float) pixel2->red / pixmap_to_compare->denominator;
                        float green2_ratio = (float) pixel2->green / pixmap_to_compare->denominator;
                        float blue2_ratio = (float) pixel2->blue / pixmap_to_compare->denominator;
                        // printf("red2_ratio %f\n", red2_ratio);
                        // printf("green2_ratio %f\n", green2_ratio);
                        // printf("blue2_ratio %f\n", blue2_ratio);

                        float red_value = (red1_ratio - red2_ratio) * (red1_ratio - red2_ratio);
                        float green_value = (green1_ratio - green2_ratio) *  (green1_ratio - green2_ratio);
                        float blue_value = (blue1_ratio - blue2_ratio) * (blue1_ratio - blue2_ratio);

                        // printf("red_value %f\n", red_value);
                        // printf("green_value %f\n", green_value);
                        // printf("blue_value %f\n", blue_value);
                        sum = red_value + green_value + blue_value; 

                        // printf("sum %f\n", sum);
                        rgb_diff_sum = rgb_diff_sum + sum;   
                        // printf("\n\n");
                        // printf("rgb_diff_sum: %f\n", rgb_diff_sum);
                }
        }
        float E = sqrt(rgb_diff_sum / (3 * min_width * min_height)); 
        printf("The value of E: %.4f\n", E); 
        
}

int determine_height(int height, int height_to_compare)
{
        int min_height = 0; 
        if (height < height_to_compare) {
                min_height = height;
        } else {
                min_height = height_to_compare;
        }

        return min_height; 
}

int determine_width(int width, int width_to_compare)
{
        int min_width = 0; 
        if (width < width_to_compare) {
                min_width = width;
        } else {
                min_width = width_to_compare;
        }

        return min_width; 
}

void check_height_width(int height1, int height2, int width1, int width2)
{
        if(abs(height1 - height2) > MAX_DIFF_HEIGHT_WIDTH || 
           abs(width1 - width2) > MAX_DIFF_HEIGHT_WIDTH) {
                
                fprintf(stderr, "Error: The difference between image dimensions is greater than 1\n");
                printf("%i\n", 1);
                exit(EXIT_FAILURE);
        }
}