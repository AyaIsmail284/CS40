/******************************************************************
 *  40image.c
 *  Aya Ismail(aismai02) and Leisha Bhandari (lbhand01)
 *  2/24/24
 *
 *  CS 40 HW 4: Arith
 *
 *  Implementation for 40image, which ...
 *
 ******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include <stdbool.h>
#include <errno.h>
#include "uarray2.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "a2methods.h"
#include "pnm.h"
#include "arith40.h"


static void (*compress_or_decompress)(FILE *input) = compress40;

int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}


static FILE *open_or_abort(char *filename, char *mode)
{       
        // printf("filename: %s\n", filename); 
        FILE *fp = fopen(filename, mode);
        // printf("file was open: %s\n", filename); 
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
     
        FILE *fp = open_or_abort(filename, "rb");    
        Pnm_ppm pixmap = Pnm_ppmread(fp, methods);     
        assert(pixmap != NULL); 
        
        fclose(fp); 
        return pixmap; 
}