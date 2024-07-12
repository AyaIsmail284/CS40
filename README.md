Project 4: Arith

Authors: Claire Levin (slevin12), Aya Ismail (aismai02)

Purpose:

- Implements the bitpack interface which provides several functionalities
  to manipulate bit fields, serving as a tool to pack multiple bit fields 
  into a single word and also unpacking a word into multiple bit fields. 

- Uses the bitpack interface to compress a given ppm image to a binary 
  image file and/or decompress the binary image back into a ppm, with
  the help of some computational conversions inspired from JPEG image 
  compression (conversion from RGB to component video space and DCT).

Acknowledgments:

- TAs: Andrew, Sam, Jian, Charlie, Dan, Tim, Randy

About Implementation:

- 40image.c: Handles command line options and calls
             compress40 or decomoress40 accordingly 
             (given code by course staff)

- compress40.c: reads in a pixmap, converts the RGB pixels into scaled 
                integers (RGB values / denominator), converts the float RGB
                values into component video space pixels, uses DCT to convert
                luma values into cosine coefficiants (a, b, c, d), 
                quantizes the component video space values of each 2 by 2 
                block of pixels and packs them into a 32-bit word and writes
                the binary compressed image to stdout. 
                
                reads in the packed words, unpacks each word, dequantizes 
                the unpacked values back into component videa space values, 
                convert these back into float RGB and then back to integer
                RGB values and writes the final ppm image to stdout. 

- bitpack.c: correctly implements bit-field operations defined in bitpack.h: 
             determines if a number fits in a certain number of bits 
             (signed or unsigned), if not, it raises an overflow exception, 
             extracts a bit field from a given word, and updates a word given
             a bit field. 


             
Hours: Design (problem analysis) - 10 hours
       Implementation (solving the problem) - 20 hours
