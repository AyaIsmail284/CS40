#include "bitpack.h"
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include "except.h"
#include "assert.h"
#include "stdio.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        uint64_t max_value = 0;

        if (width == 64) {
                max_value = ~0;
        } else {
                max_value = ((uint64_t)1 << width) - 1;
        }

        return (n <= max_value);
}


bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
        if (width != 0) {
                width--;
        }
        /* Subtract 1 because signed can only go to 2^(width-1) + or - */
        int64_t max_pos = 0;
        int64_t max_neg = 0;

        if (width == 64) {
                max_pos = ~0;
        } else {
                max_pos = ((uint64_t)1 << width) - 1;
        }
        max_neg = -(max_pos + 1);

        return ((n >= max_neg && n <= max_pos));
}


uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(lsb <= 64);
        assert((width + lsb) <= 64);
        uint64_t mask = (((uint64_t)1 << width) - 1) << lsb; 
        if (width == 64) {
                mask = ~0;
                mask = mask << lsb;
        }
        return ((word & mask) >> lsb);
}


int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(lsb <= 64);
        assert((width + lsb) <= 64);
        uint64_t raw_result = Bitpack_getu(word, width, lsb);
        // printf("Got raw result: %lu\n", raw_result);
        int64_t result = 0;
        int64_t complement_ones = ~0;
        /* This shift is here so that we can avoid overwriting any of our actual value with 1s*/
        complement_ones = complement_ones << width;
        /* Set this to 0 because we don't want the later if statement going based on an uninitialized value */
        uint64_t MSB = 0;
        if (lsb != 0 || width != 0) {
                MSB = Bitpack_getu(word, 1, (lsb + width) - 1);
        }
        if (MSB == 1) {
                // printf("Current number appears to be negative\n");
                /* if the number is negative, add the complementary ones*/
                result = complement_ones | raw_result;
        } else {
                /* If not negative, we can just return the raw unsigned result */
                result = raw_result;
        }
        return (result);
}


uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
        // Step 0: make sure everything is good (asserts etc)
        assert(width <= 64);
        assert(lsb <= 64);
        assert((width + lsb) <= 64);
        // printf("Value is %lu\n", value);
        if (!(Bitpack_fitsu(value, width))) {
                RAISE(Bitpack_Overflow);
        }
        // Step 1: Create a mask to access our chunk of word
        uint64_t mask = 0;
        if (width != 64) {
                mask = (((uint64_t)1 << width) - 1) << lsb; 
        } else {
                mask = ~mask;
        }
        // Step 2: Create an anti-mask so we can zero out our chunk of word
        uint64_t antimask = ~mask;
        word &= antimask;
        // Step 3: use |= to ensure all word bits go through, along w mask bits
        word |= (value << lsb);
        // Step 4: return word
        return word;
}


uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value)
{
        assert(width <= 64);
        assert(lsb <= 64);
        assert((width + lsb) <= 64);

        if (!(Bitpack_fitss(value, width))) {
                RAISE(Bitpack_Overflow);
        }

        int64_t mask = ~0;
        if (width != 64) {
                mask = (((uint64_t)1 << width) - 1) << lsb; 
        } else {
                mask = ~mask;
        }

        // Create an anti-mask so we can zero out our chunk of word
        int64_t antimask = ~mask;
        word &= antimask;
        // use |= to ensure all word bits go through, along w mask bits
        word |= (value << lsb);
     
        return word;
}