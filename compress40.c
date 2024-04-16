#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "compress40.h"
#include "assert.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "pnm.h"
#include "arith40.h"
#include "uarray2b.h"
#include "bitpack.h"

/* This is a data type to hold our float-converted rgb pixels */
typedef struct Pnm_rgb_float {
        float red, green, blue;
} *Pnm_rgb_float;

typedef struct vid_pxl {
        float y, pb, pr;
} *vid_pxl;

typedef struct qblock {
        int qb, qc, qd;
        unsigned qa, chroma_pb, chroma_pr;
} *qblock;


void pix_trimmer(Pnm_ppm *pixmap);
void apply_float_convert(int col, int row, UArray2b_T arr, void *elem, void *cl);
void apply_component_conversion(int col, int row, UArray2b_T array2b, void *elem, void *cl);
UArray2b_T convert_to_floats(Pnm_ppm pixmap);
UArray2b_T convert_float_to_component(UArray2b_T rgb_floats_arr);
void apply_undo_convert_floats(int col, int row, UArray2b_T array2b, void *ptr, void *cl_int_pixmap);
void apply_undo_comp_conv(int col, int row, UArray2b_T array2b, void *elem, void *cl);
float avg(float v1, float v2, float v3, float v4);
UArray2b_T quant_and_pack_words(UArray2b_T component_arr);
void apply_comp_packer(int col, int row, UArray2b_T array2b, void *elem, void *cl);
int make_signed_int(float x);
struct qblock quantize(float a, float b, float c, float d, float pb, float pr);
void dequant_block(qblock block, vid_pxl a, vid_pxl b, vid_pxl c, vid_pxl d);
float undo_quantization(int x);
float clamp_value(float min, float max, float x);
uint32_t pack_block(qblock block);
struct qblock unpack_block(uint32_t word);
UArray2b_T make_pix_array(UArray2b_T words);
void apply_unpack_words(int col, int row, UArray2b_T array2b, void *elem, void *cl);
void output_image(UArray2b_T words);
void apply_output_words(int col, int row, UArray2b_T array2b, void *elem, void *cl);
void apply_words_from_file(int col, int row, UArray2b_T array2b, void *elem, void *cl);

void
check_print_rgb_int(int i, int j, UArray2b_T array2b, void *elem, void *p2) 
{
        assert((array2b != NULL) && (elem != NULL));
        Pnm_rgb rgb = elem; 
              
        printf("rgb red: %d\n", rgb->red);
        printf("rgb green: %d\n", rgb->green);  
        printf("rgb blue: %d\n", rgb->blue); 

        printf("ar[%d,%d]\n", i, j);
        (void) array2b; 
        (void) p2; 
}


void
check_print_rgb_float(int i, int j, UArray2b_T array2b, void *elem, void *p2) 
{
        assert((array2b != NULL) && (elem != NULL));
        Pnm_rgb_float rgb = elem; 
              
        printf("rgb red: %f\n", rgb->red);
        printf("rgb green: %f\n", rgb->green);  
        printf("rgb blue: %f\n", rgb->blue); 

        printf("ar[%d,%d]\n", i, j);
        (void) array2b; 
        (void) p2; 

}


void
check_print_component(int i, int j, UArray2b_T array2b, void *elem, void *p2) 
{
        assert((array2b != NULL) && (elem != NULL));
        vid_pxl vid_pix = elem; 
              
        printf("Y: %f\n", vid_pix->y);
        printf("Pb: %f\n", vid_pix->pb);  
        printf("Pr: %f\n", vid_pix->pr); 

        printf("ar[%d,%d]\n", i, j);
        (void) array2b; 
        (void) p2; 
}


/********************** compress40 **********************
 * Compresses
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
extern void compress40(FILE *input)
{
        assert(input != NULL); 
        
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods != NULL);
    
        Pnm_ppm pixmap = Pnm_ppmread(input, methods);
        assert(pixmap != NULL);

        pix_trimmer(&pixmap);
        /* This holds the result of turning the RGBs into floats */
        // UArray2b_map(pixmap->pixels, check_print_rgb_int, NULL);
        UArray2b_T rgb_floats_arr = convert_to_floats(pixmap); 
        assert(rgb_floats_arr != NULL); 
        /* Just some variables so we don't have to worry about line lengths*/
        // int width, height;
        // width = pixmap->width;
        // height = pixmap->height;
        // size_rgb = sizeof(struct Pnm_rgb);
        // size_rgb_f = sizeof(struct Pnm_rgb_float);
        /* Hold the component floats */
        UArray2b_T component_arr = convert_float_to_component(rgb_floats_arr);
        // UArray2b_map(rgb_floats_arr, check_print_rgb_float, NULL);
        /* Hold the comps which have been quantized and packed */
        UArray2b_T quant_packed_arr = quant_and_pack_words(component_arr);
        // UArray2b_T depack_dequant_array = make_pix_array(avg_deavg_array);
        // /* Hold the re-RGB'd floats*/
        // UArray2b_T reRGB_flts = UArray2b_new(width, height, size_rgb_f, 2);
        // /* re-RGB the floats */
        // UArray2b_map(depack_dequant_array, apply_undo_comp_conv, &reRGB_flts);
        // UArray2b_T junk = pixmap->pixels;
        // /* Assign this new map to our original pixmap's pixels */
        // pixmap->pixels = UArray2b_new(width, height, size_rgb, 1);
        // /* Undo the float conversion on the pixel map*/
        // UArray2b_map(reRGB_flts, apply_undo_convert_floats, &pixmap);
        // /* Open our destination file to write to (so we can ppmdiff) */
        // FILE *test_fptr = fopen("compress40_output.ppm", "w");
        /* Write out our finish test image */
        //Pnm_ppmwrite(test_fptr, pixmap);
        output_image(quant_packed_arr);
        UArray2b_free(&rgb_floats_arr);
        UArray2b_free(&component_arr);
        UArray2b_free(&quant_packed_arr);
        // UArray2b_free(&depack_dequant_array);
        // UArray2b_free(&reRGB_flts);
        // UArray2b_free(&junk);
        Pnm_ppmfree(&pixmap);
        //fclose(test_fptr);
}


/********************** pix_trimmer **********************
 * Trims a pixmap if any dimension isn't divisble by 2
 * 
 * inputs: Ptr to a pixmap to trim
 *         
 * return: nothing
 *
 * expects: a valid pixmap ptr to trim (if needed)
 * 
 *********************************************************/
void pix_trimmer(Pnm_ppm *pixmap)
{
        assert(pixmap != NULL);
        if ((*pixmap)->width % 2 != 0) {
                (*pixmap)->width--;
        }
        if ((*pixmap)->height % 2 != 0) {
                (*pixmap)->height--;
        }
}


UArray2b_T convert_to_floats(Pnm_ppm pixmap)
{       
        assert((pixmap != NULL));
        Pnm_ppm float_pixmap = malloc(sizeof(struct Pnm_ppm));
        assert(float_pixmap != NULL);
        /* Creates vars to hold everything, so we can shorten other lines */
        int width, height, itemsize;
        width = pixmap->width;
        height = pixmap->height;
        itemsize = sizeof(struct Pnm_rgb_float);
        UArray2b_T rgb_floats_arr = UArray2b_new(width, height, itemsize, 2);
        assert(rgb_floats_arr != NULL);
        /* Set up the pixmap we'll use for this */
        float_pixmap->denominator = pixmap->denominator; 
        float_pixmap->width = width; 
        float_pixmap->height = height; 
        float_pixmap->methods = pixmap->methods;
        /* Free float pixmap later */
        float_pixmap->pixels = rgb_floats_arr;
        /* Use an apply function to do the actual float conversion */
        UArray2b_map(pixmap->pixels, apply_float_convert, &float_pixmap);
        UArray2b_T test = UArray2b_new(width, height, itemsize, 2);
        float_pixmap->pixels = test;
        Pnm_ppmfree(&float_pixmap);
        return rgb_floats_arr; 
}


void apply_float_convert(int col, int row, UArray2b_T arr, void *ptr, void *cl)
{
        assert((arr != NULL) && (ptr != NULL));
        /* Get a pixmap ptr from our closure ptr */
        Pnm_ppm float_pixmap = *(Pnm_ppm *)cl;
        /* get a pnmrgb ptr to hold our current element */
        Pnm_rgb int_rgb = ptr;
        /* Just grab some basic information about the pixmap */
        int height = float_pixmap->height;
        int width =  float_pixmap->width;
        int denom = float_pixmap->denominator;
        UArray2b_T pixls = float_pixmap->pixels;
        /* If the current index is part of a trimmed row/col, skip it now */
        if (col == width || row == height) {
                return;
        }
        /* Grab the address of the RGB float at the equivalent index of dest*/
        Pnm_rgb_float float_rgb = (Pnm_rgb_float)UArray2b_at(pixls, col, row);
        /* Do the actual assignment of the vars */
        float_rgb->red = (float)int_rgb->red / denom;
        float_rgb->green = (float)int_rgb->green / denom;
        float_rgb->blue = (float)int_rgb->blue / denom;
        /* Some data loss here - float division loses small trailing digits */
}


void apply_undo_convert_floats(int col, int row, UArray2b_T array2b,
                       void *ptr, void *cl_int_pixmap) 
                       
{
        assert((array2b != NULL) && (ptr != NULL));
        /* Get a pixmap ptr from our closure ptr */
        Pnm_ppm int_pixmap = *(Pnm_ppm *)cl_int_pixmap;
        /* get a rgb float ptr to hold our current element */
        Pnm_rgb_float float_rgb = ptr;
        /* Grab basic information about pixmap which we may need later */
        int height = int_pixmap->height;
        int width = int_pixmap->width; 
        int denom = int_pixmap->denominator;
        /* If the current index is part of a trimmed row/col, skip it now */
        if (col == width || row == height) {
                return;
        }
        Pnm_rgb int_rgb = (Pnm_rgb) UArray2b_at(int_pixmap->pixels, col, row);
        /* Do the denominator multiplication */
        int temp_red = float_rgb->red * denom;
        int temp_green = float_rgb->green * denom;
        int temp_blue = float_rgb->blue * denom;
        /* Assign the clamped temp values to our destination rgb object */
        int_rgb->red   = clamp_value(0, (float)denom, (float)temp_red);
        int_rgb->green = clamp_value(0, (float)denom, (float)temp_green);
        int_rgb->blue  = clamp_value(0, (float)denom, (float)temp_blue);
        /* We clamp with casting because it doesn't use additional lines */
}


UArray2b_T convert_float_to_component(UArray2b_T rgb_floats_arr)
{
        // TODO: Add some asserts to catch any bad stuff that gets passed in
        /* Step 1: Create Uarray2 of equal dimensions to the RGB floats arr */
        int height = UArray2b_height(rgb_floats_arr);
        int width = UArray2b_width(rgb_floats_arr);
        int pix_size = sizeof(struct vid_pxl);
        UArray2b_T comp_arr = UArray2b_new(width, height, pix_size, 2);
        /* Step 2: convert values and save to component array */
        UArray2b_map(rgb_floats_arr, apply_component_conversion, &comp_arr); 
        /* Step 3: Return array */
        return comp_arr;
}


void apply_component_conversion(int col, int row, UArray2b_T array2b, void *elem, void *cl) 
{
        assert((array2b != NULL) && (elem != NULL)); 
        /* Create the holder values for our WIP numbers */
        float red, green, blue;
        /* Create temp values */
        Pnm_rgb_float current_float_item = elem;
        /* Grab current float RGB vals */
        red = current_float_item->red;
        green = current_float_item->green;
        blue = current_float_item->blue;
        /* Cast closure ptr in a way thats easy to use */
        UArray2b_T *comp_arr = (UArray2b_T *)cl;
        /* Create current pixel */
        struct vid_pxl current_pixel;
        /* Calculate y, pb, and pr */
        float y = 0.299 * red + 0.587 * green + 0.114 * blue;
        float pb = -0.168736 * red - 0.331264 * green + 0.5 * blue;
        float pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;
        /* Save the y, pb, and pr */
        current_pixel.y = y;
        current_pixel.pb = pb;
        current_pixel.pr = pr;
        // printf("Current pixel's y is: %f\n", current_pixel.y);
        // printf("Current pixel's pb is: %f\n", current_pixel.pb);
        // printf("Current pixel's pr is: %f\n", current_pixel.pr);
        *(vid_pxl)UArray2b_at(*comp_arr, col, row) = current_pixel;
        (void) array2b;
}


void apply_undo_comp_conv(int col, int row, UArray2b_T array2b, void *elem, void *cl) 
{
        assert((array2b != NULL) && (elem != NULL)); 
        /* Create our floats to hold the WIP numbers */
        float red, green, blue, y, pb, pr;
        vid_pxl current_comp_item = elem;
        /* Grab the current Y Pb Pr*/
        y = current_comp_item->y;
        pb = current_comp_item->pb;
        pr = current_comp_item->pr;
        /* Create an RGB struct to hold our values */
        struct Pnm_rgb_float curr_rgb;
        /* Calculate the RGB values */
        red = 1.0 * y + 0.0 * pb + 1.402 * pr;
        green = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        blue = 1.0 * y + 1.772 * pb + 0.0 * pr;
        /* Save RGB to our new object */
        curr_rgb.red = red;
        curr_rgb.green = green;
        curr_rgb.blue = blue;
        /* Cast cl ptr as UArray2b_T ptr, use that to get address of rgb    *
         * object, cast + dereference that, assign our new values           */
        *(Pnm_rgb_float)UArray2b_at(*(UArray2b_T *)cl, col, row) = curr_rgb;
        /* Kind of an ugly line but it gets a lot done in a small space */
        (void) array2b;
}


UArray2b_T quant_and_pack_words(UArray2b_T component_arr)
{
        // TODO: Add some asserts to catch any bad stuff that gets passed in
        int width = UArray2b_width(component_arr);
        int height = UArray2b_height(component_arr);
        // int pix_size = sizeof(struct vid_pxl);
        /* Step 1: Create UArray2 of words to hold our final output */
        // Size should be number of bits/bytes/? in a word
        if (width == 1) {
                width++;
        } 
        if (height == 1) {
                height++;
        }
        UArray2b_T word_array = UArray2b_new((width / 2), (height / 2), sizeof(uint32_t), 1);

        // This is a temporary array to hold the vid pixels that have been 
        // quantized back and forth
        // UArray2b_T word_array = UArray2b_new(width, height, pix_size, 2);

        /* Step 2: For each block in comp_arr, calculate the values via our *
         *         pack fxn, store packed word in step 1 array              */
        UArray2b_map(component_arr, apply_comp_packer, &word_array);
        /* Step 3: return step 1 array when thats done */
        return word_array;
}


void apply_comp_packer(int col, int row, UArray2b_T array2b, void *elem, void *cl) 
{
        /* If not at the start of a block, return */
        if (col % 2 != 0 || row % 2 != 0) {
                return;
        }
        /* Grab the four pixels in the block */
        struct vid_pxl pixl[4];
        pixl[0] = *(vid_pxl) elem;
        pixl[1] = *(vid_pxl) UArray2b_at(array2b, col + 1, row);
        pixl[2] = *(vid_pxl) UArray2b_at(array2b, col, row + 1);
        pixl[3] = *(vid_pxl) UArray2b_at(array2b, col + 1, row + 1);
        /* Clamp the Pb and Pr for each of our pixels */
        for (int i = 0; i < 4; i++) {
                pixl[i].pb = clamp_value(-0.5, 0.5, pixl[i].pb);
                pixl[i].pr = clamp_value(-0.5, 0.5, pixl[i].pr);
        }
        /* Calculate Avg Pb and Pr throughout the block */
        float avg_pb = avg(pixl[0].pb, pixl[1].pb, pixl[2].pb, pixl[3].pb);
        float avg_pr = avg(pixl[0].pr, pixl[1].pr, pixl[2].pr, pixl[3].pr);
        /* Dct y1, y2, y3, and y4, to get our abcd */
        float a = (pixl[3].y + pixl[2].y + pixl[1].y + pixl[0].y) / 4.0;
        float b = (pixl[3].y + pixl[2].y - pixl[1].y - pixl[0].y) / 4.0;
        float c = (pixl[3].y - pixl[2].y + pixl[1].y - pixl[0].y) / 4.0;
        float d = (pixl[3].y - pixl[2].y - pixl[1].y + pixl[0].y) / 4.0;
        /* Use qblock struct to make quantized blocks easier to pass */
        struct qblock curr_block = quantize(a, b, c, d, avg_pb, avg_pr);
        // printf("\n \n \nWE ARE NOW STARTING A NEW BLOCK \n \n \n");

        // printf("Pre packing values are:\n");
        // printf("Pb and Pr are: %u %u\n", curr_block.chroma_pb, curr_block.chroma_pr);
        // printf("A is %u\n", curr_block.qa);
        // printf("B is %i\n", curr_block.qb);
        // printf("C is %i\n", curr_block.qc);
        // printf("D is %i\n", curr_block.qd);
        // printf("Attempting to pack block\n");
        uint32_t word = pack_block(&curr_block);
        // printf("The word is: %u\n", word);
        UArray2b_T wordarray = *(UArray2b_T *)cl;
        // printf("Making wordptr\n");
        // printf("Col + row are: %i, %i\n", col, row);
        *(uint32_t *)UArray2b_at(wordarray, col / 2, row / 2) = word;
        // // printf("Packed successfully\n");
        // // printf("Attempting to unpack block\n");
        // struct qblock empty_block = unpack_block(word);
        // // printf("Unpacked successfully\n");
        // // printf("Attempting to print unpacked block contents\n");
        // // printf("Pb and Pr are: %u %u\n", empty_block.chroma_pb, empty_block.chroma_pr);
        // // printf("A is %u\n", empty_block.qa);
        // // printf("B is %i\n", empty_block.qb);
        // // printf("C is %i\n", empty_block.qc);
}

struct qblock quantize(float a, float b, float c, float d, float pb, float pr) 
{
        struct qblock quant_block;
        /* Chroma the Pb and Pr */
        quant_block.chroma_pb = Arith40_index_of_chroma(pb);
        quant_block.chroma_pr = Arith40_index_of_chroma(pr);
        /* Convert abcd and quantize correctly */
        quant_block.qa = round(a * 511);
        quant_block.qb = make_signed_int(b);
        quant_block.qc = make_signed_int(c);
        quant_block.qd = make_signed_int(d);
        /* Return the block, since it's finished */
        return quant_block;
}


float avg(float v1, float v2, float v3, float v4) 
{
        return ((v1 + v2 + v3 + v4) / 4.0);
}


int make_signed_int(float x)
{
        /* clamp x */
        x = clamp_value(-0.3, 0.3, x);
        /* do the multiplication, round, return*/
        return (round(x * 50));
}

uint32_t pack_block(qblock block)
{
        /* Have word start off as all 1s because it's easier to visualize */
        uint32_t word = 0;

        // Pack Pb/Pr/a as unsigned, dcb as signed

        // **** put bit widths in constants (9, 5, 4)
        //printf("packing pr\n");
        word = Bitpack_newu(word, 4, 0, block->chroma_pr);
        //printf("packing pb\n");
        word = Bitpack_newu(word, 4, 4, block->chroma_pb);
        //printf("packing d\n");
        word = Bitpack_news(word, 5, 8, block->qd);
        //printf("packing c\n");
        word = Bitpack_news(word, 5, 13, block->qc);
        //printf("packing b\n");
        word = Bitpack_news(word, 5, 18, block->qb);
        //printf("packing a\n");
        word = Bitpack_newu(word, 9, 23, block->qa);
        //printf("Word before returning from pack is: %u\n", word);
        return word;
}

struct qblock unpack_block(uint32_t word)
{
        /* Have word start off as all 1s because it's easier to visualize */
        struct qblock block;

        // Unpack Pb/Pr/a as unsigned, dcb as signed

        block.chroma_pr = Bitpack_getu(word, 4, 0);
        block.chroma_pb = Bitpack_getu(word, 4, 4);
        block.qd = Bitpack_gets(word, 5, 8);
        block.qc = Bitpack_gets(word, 5, 13);
        block.qb = Bitpack_gets(word, 5, 18);
        block.qa = Bitpack_getu(word, 9, 23);
        // printf("Chroma pr is %u\n", block.chroma_pr);
        // printf("qd is %i\n", block.qd);
        // printf("qc is %i\n", block.qc);
        // printf("qb is %i\n", block.qb);
        // printf("qa is %u\n", block.qa);

        return block;
}

UArray2b_T make_pix_array(UArray2b_T words)
{
        int width = (UArray2b_width(words) * 2);
        int height = (UArray2b_height(words) * 2);
        // printf("WIdth of pix array is %i\n", width);
        // printf("Height of pix array is %i\n", height);
        UArray2b_T pix = UArray2b_new(width, height, sizeof(struct vid_pxl), 2);
        // Iterate through words
        UArray2b_map(words, apply_unpack_words, &pix); 

        return pix; 
}



void apply_unpack_words(int col, int row, UArray2b_T array2b, void *elem, void *cl)
{
        (void) array2b;
        // Turn current word into 1 qblock
        // Turn current qblock into 4 dequanted vid pixels
        // Store dequanted vid pixels in destination array
        col = col * 2;
        row = row * 2;
        UArray2b_T pixels = *(UArray2b_T *)cl;
        uint32_t curr_word = *(uint32_t *)elem;
        struct qblock new_block;
        new_block = unpack_block(curr_word);
        // printf("Trying to grab pixel locations\n");
        vid_pxl p1 = UArray2b_at(pixels, col, row);
        vid_pxl p2 = UArray2b_at(pixels, col + 1, row);
        vid_pxl p3 = UArray2b_at(pixels, col, row + 1);
        vid_pxl p4 = UArray2b_at(pixels, col + 1, row + 1);

        // printf("made it here\n"); 
        dequant_block(&new_block, p1, p2, p3, p4);
}


// When this guy gets given a block, dequantizes it and saves 4 pixels at given addresses
void dequant_block(qblock block, vid_pxl a, vid_pxl b, vid_pxl c, vid_pxl d)
{
        // printf("test\n");
        /* Step 1: Process our abcds to prep them for DCT */
        float a2 = ((float)block->qa / 511);
        float b2 = undo_quantization(block->qb);
        float c2 = undo_quantization(block->qc);
        float d2 = undo_quantization(block->qd);
        /* Step 2: ABCD get put into inverse DCT */
        float Y[4];
        Y[0] = a2 - b2 - c2 + d2; // in single block test file, should be close to 0.416134
        Y[1] = a2 - b2 + c2 - d2; // in single block test file, should be close to 0.927612
        Y[2] = a2 + b2 - c2 - d2; // in single block test file, should be close to 0.353104
        Y[3] = a2 + b2 + c2 + d2; // in single block test file, should be close to 0.893836
        /* Step 3: PB and PR = chroma of index (pb avg) (pr avg) */
        float pb = Arith40_chroma_of_index(block->chroma_pb);
        float pr = Arith40_chroma_of_index(block->chroma_pr);
        /* Step 4: Put our Ys and pb/pr into vid pixel block again */
        vid_pxl pixls[4] = {a, b, c, d};
        for (int i = 0; i < 4; i++) {
                pixls[i]->y = Y[i];
                pixls[i]->pb = pb;
                pixls[i]->pr = pr;
                // printf("Pix%i values:\nY=%f\nPb=%f\nPr=%f\n", i, pixls[i]->y, pixls[i]->pb, pixls[i]->pr);
        }
}


float undo_quantization(int x)
{
        // No reason for this to be a helper func but it makes it a little 
        // quicker to change later if we find any edge cases/need new behavior
        return ((float)x / 50);
}


float clamp_value(float min, float max, float x)
{
        /* If x is below our lower bound, normalize up */
        if (x < min) {
                x = min;
        }
        /* If x is below our upper bound, normalize down */
        if (x > max) {
                x = max;
        }
        /* Send the result back (if it didn't need clamping, that's fine) */
        return x;
}





void output_image(UArray2b_T words)
{
        int width = UArray2b_width(words) * 2;
        int height = UArray2b_height(words) * 2;
        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        printf("\n");
        UArray2b_map(words, apply_output_words, NULL);
}

void apply_output_words(int col, int row, UArray2b_T array2b, void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) cl;
        (void) array2b;
        uint32_t current_word = *(uint32_t *)elem;

        uint8_t part1 = Bitpack_getu(current_word, 8, 24);
        uint8_t part2 = Bitpack_getu(current_word, 8, 16);
        uint8_t part3 = Bitpack_getu(current_word, 8, 8);
        uint8_t part4 = Bitpack_getu(current_word, 8, 0);
        putchar(part1);
        putchar(part2);
        putchar(part3);
        putchar(part4);
}


/********************** decompress40 **********************
 * Decompresses
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
extern void decompress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods != NULL);
        /* Pull in header */
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        // Populate this once other stuff works
        Pnm_ppm final_image = malloc(sizeof(struct Pnm_ppm));
        final_image->denominator = 255;
        final_image->width = width;
        final_image->height = height;
        final_image->methods = methods;

        // Use helpers to do the conversion back to an image:

        // Pull in words (in 8 bit chunks)
        UArray2b_T words = UArray2b_new(width / 2, height / 2, sizeof(uint32_t), 1);
        UArray2b_map(words, apply_words_from_file, input);
        // Try to make the words into component
        UArray2b_T component = make_pix_array(words);
        // UArray2b_map(component, check_print_component, NULL);
        /* Hold the re-RGB'd floats*/
        UArray2b_T reRGB_flts = UArray2b_new(width, height, sizeof(struct Pnm_rgb_float), 2);
        /* re-RGB the floats */
        UArray2b_map(component, apply_undo_comp_conv, &reRGB_flts);
        /* Assign this new map to our original pixmap's pixels */
        final_image->pixels = UArray2b_new(width, height, sizeof(struct Pnm_rgb), 1);
        /* Undo the float conversion on the pixel map*/
        UArray2b_map(reRGB_flts, apply_undo_convert_floats, &final_image);
        // ppmwrite the result
        Pnm_ppmwrite(stdout, final_image);
}



void apply_words_from_file(int col, int row, UArray2b_T array2b, void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) array2b;
        // Assert correct number of words read + for every word use assert bitpack fitu

        // Grab the word chunks
        uint8_t test4 = getc((FILE*)cl);
        uint8_t test3 = getc((FILE*)cl);
        uint8_t test2 = getc((FILE*)cl);
        uint8_t test1 = getc((FILE*)cl);   
        // Make the word                     
        uint32_t word = ~0;
        word = Bitpack_newu(word, 8, 0, test1);
        word = Bitpack_newu(word, 8, 8, test2);
        word = Bitpack_newu(word, 8, 16, test3);
        word = Bitpack_newu(word, 8, 24, test4);
        // Put word into array
        *(uint32_t *)elem = word;
}



















// REMOVED CODE BELOW THIS LINE, ONLY HERE IN CASE ITS NEEDED AGAIN LATER:


        // if (temp_red < 0) {
        //         temp_red = 0;
        // }
        // if (temp_blue < 0) {
        //         temp_blue = 0;
        // }       
        // if (temp_green < 0) {
        //         temp_green = 0;
        // }
        // if (temp_red > denom) {
        //         temp_red = denom;
        // }
        // if (temp_blue > denom) {
        //         temp_blue = denom;
        // }       
        // if (temp_green > denom) {
        //         temp_green = denom;
        // }