#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "bitpack.h"
#include "assert.h"


void test_fitu_no_width_true()
{
        assert(Bitpack_fitsu(0, 0));
        printf("unsigned 0 fits in 0 passed\n");
}

void test_fitu_no_width_false()
{
        assert(!(Bitpack_fitsu(1, 0))); // passing in hex 
        printf("unsigned 1 fits in 0 failed\n");
}
void test_fitu_1_test()
{
        assert((Bitpack_fitsu(1, 1)));
        printf("unsigned 1 fits in 1 passed\n");
}

void test_fitu_true()
{
        assert(Bitpack_fitsu(1, 3)); // passing in hex 
        printf("unsigned 1 fits in 3 passed\n");
}

void test_fitu_false()
{
        assert(!(Bitpack_fitsu(8, 3)));
        printf("unsigned 8 fits in 3 failed\n");
}

void test_fitu_15_test()
{
        assert((Bitpack_fitsu(15, 4)));
        printf("unsigned 15 fits in 4 passed\n");
}

void test_fitu_0_width_test()
{
        assert(!(Bitpack_fitsu(4, 2)));
        printf("unsigned 4 fits in 2 failed\n");
}

void test_fits_positive_true() {
        assert(Bitpack_fitss(127, 8));
        printf("signed 127 fits in 8 passed\n");
}

void test_fits_positive_false() {
        assert(!(Bitpack_fitss(128, 8)));
        printf("signed 128 fits in 8 failed\n");
}

void test_fits_negative_true() {
        assert(Bitpack_fitss(-128, 8));
        printf("signed -128 fits in 8 passed\n");
}

void test_fits_negative_false() {
        assert(!(Bitpack_fitss(-129, 8)));
        printf("signed -129 fits in 8 failed\n");
}

void test_fits_negative_false_2_bit() {
        assert((Bitpack_fitss(1, 2)));
        printf("signed 1 fits in 2 passed\n");
}

void test_fit_signed_0_1() {
        assert((Bitpack_fitss(0, 1)));
        printf("signed 0 fits in 1 passed\n");
}

void test_fits_negative_false_1_bit() {
        assert((Bitpack_fitss(-1, 1)));
        printf("signed -1 fits in 1 passed\n");
}

void test_fits_negative_false_0_0() {
        assert((Bitpack_fitss(0, 0)));
        printf("signed 0 fits in 0 passed\n");
}

void test_fit_signed_0_neg_1() {
        assert((!Bitpack_fitss(1, 1)));
        printf("signed 1 fits in 1 failed\n");
}

void test_fit_signed_1_0() {
        assert(!(Bitpack_fitss(1, 0)));
        printf("signed 1 fits in 0 failed\n");
}

void get_u_test_15() {
        uint64_t test = Bitpack_getu(44893, 4, 8);
        printf("was able to grab 4 bits from test number 1010111101011101. test is: %lu\n", test);
        assert(test == 15);
}

void get_u_test_width_64() {
        uint64_t test = Bitpack_getu(~0, 62, 2);
        uint64_t test2 = ~0;
        test2 = test2 >> 2;
        printf("was able to grab 4 bits from test number all 1s. Test is: %lu Test2 is: %lu\n", test, test2);
        assert(test == test2);
}

void get_u_test_7() {
        uint64_t test = Bitpack_getu(44893, 4, 2);
        printf("was able to grab 4 bits from test number 1010111101011101. test is: %lu\n", test);
        assert(test == 7);
}

void get_u_test_spec() {
        uint64_t test = Bitpack_getu(0x3f4, 6, 2);
        printf("was able to grab 6 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 61);
}

void get_u_test_edge() {
        uint64_t test = Bitpack_getu(0x3f4, 4, 0);
        printf("was able to grab 4 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 4);
}

void get_u_test_edge_2() {
        uint64_t test = Bitpack_getu(0x3f4, 12, 0);
        printf("was able to grab 12 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 0x3f4);
}

void get_u_test_edge_3() {
        uint64_t test = Bitpack_getu(0x3f4, 6, 2);
        printf("was able to grab 6 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 61);
}

void get_u_test_edge_4() {
        uint64_t test = Bitpack_getu(0x3f4, 0, 43);
        printf("was able to grab 0 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 0);
}
void get_u_test_edge_5() {
        uint64_t test = Bitpack_getu(0x3f4, 1, 63);
        printf("was able to grab 0 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 0);
}

void get_u_test_spec_edited() {
        uint64_t test = Bitpack_getu(0x3f4, 7, 1);
        printf("was able to grab 7 bits from test number 0x3f4. test is: %lu\n", test);
        assert(test == 122);
}

void get_s_test_width_64() {
        int64_t test = Bitpack_gets(~0, 64, 0);
        int64_t test2 = ~0;
        // test2 = test2 >> 2;
        printf("was able to grab 4 bits from test number all 1s. Test is: %lu Test2 is: %lu\n", test, test2);
        assert(test == test2);
}



void get_s_test_spec() {
        int64_t test = Bitpack_gets(0x3f4, 6, 2);
        printf("was able to grab 6 bits from test number 0x3f4. test is: %li\n", test);
        assert(test == -3);
}

void get_s_test_spec_1() {
        int64_t test = Bitpack_gets(0x00f, 5, 0);
        printf("was able to grab 5 bits from test number 0x00f. test is: %li\n", test);
        assert(test == 15);
}

void get_s_test_spec_2() {
        int64_t test = Bitpack_gets(0x00f, 3, 0);
        printf("was able to grab 3 bits from test number 0x00f. test is: %li\n", test);
        assert(test == -1);
}

void get_s_test_spec_3() {
        // This tests what happens if the LSB is waaaaaaaaay out in the 0s preceding the actual value
        int64_t test = Bitpack_gets(13, 3, 15);
        printf("was able to grab 3 bits from test number 13. test is: %li\n", test);
        assert(test == 0);
}

void get_s_test_spec_4() {
        // 57 is 111001 in binary. Bitpack gets should grab 001 (dec 1)
        int64_t test = Bitpack_gets(57, 3, 0);
        printf("was able to grab 3 bits from test number 0x3f4. test is: %li\n", test);
        assert(test == 1);
}

void get_u_s_test_comparison() {
        // 0x3f4 is 1111110100 in binary. Bitpack gets should grab 101 (which would be -3 in 2s complement)
        // 0x3f4 is 1111110100 in binary. Bitpack getu should grab 101 (which would be 5 in regular binary)
        int64_t test = Bitpack_gets(0x3f4, 3, 2);
        uint64_t test2 = Bitpack_getu(0x3f4, 3, 2);
        printf("was able to grab 3 bits from test number 0x3f4. test is: %li\n", test);
        assert(test == -3 && test2 == 5);
}

void get_u_s_test_comparison_positive() {
        // 0x3f4 is 1111110100 in binary. Bitpack gets should grab 0100 (which would be 4 in 2s complement)
        // 0x3f4 is 1111110100 in binary. Bitpack getu should grab 0100 (which would be 4 in regular binary)
        int64_t test = Bitpack_gets(0x3f4, 4, 0);
        uint64_t test2 = Bitpack_getu(0x3f4, 4, 0);
        printf("was able to grab 4 bits from test number 0x3f4. test is: %li\n", test);
        assert(test == 4 && test2 == 4);
}

void new_u_test_1() {
        // ~0 is a short way to write all 1s in as many bits as a type can fit
        uint64_t test_word = ~0;
        assert(test_word == 0xFFFFFFFFFFFFFFFF);

        uint64_t test2 = Bitpack_newu(test_word, 4, 4, 8);
        // printf("test2 after adding 8: %lu\n", test2);
        assert(test2 == 0xFFFFFFFFFFFFFF8F);

        test2 = Bitpack_getu(test2, 4, 4);
        printf("was able to write and grab value: %lu\n", test2);
        assert(test2 == 8);
}

void new_u_test_1_width_5() {
        // ~0 is a short way to write all 1s in as many bits as a type can fit
        uint64_t test_word = ~0;
        assert(test_word == 0xFFFFFFFFFFFFFFFF);

        uint64_t test2 = Bitpack_newu(test_word, 5, 4, 8);
        // printf("test2 after adding 8: %lu\n", test2);
        assert(test2 == 0xFFFFFFFFFFFFFE8F);

        // test2 = Bitpack_getu(test2, 5, 4);
        // printf("was able to write and grab value: %lu\n", test2);
        // assert(test2 == 8);
        test2 = Bitpack_getu(test2, 7, 4);
        printf("was able to write and grab value: %lu\n", test2);
        assert(test2 == 104);
}

void new_u_test_2() {
        uint64_t test_word = 0;
        uint64_t test2 = Bitpack_newu(test_word, 4, 4, 3);
        assert(test2 = 0x0000000000000030); 
        test2 = Bitpack_getu(test2, 4, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == 0x3);
}

void new_u_test_3() {
        uint64_t test_word = 0;
        uint64_t test2 = Bitpack_newu(test_word, 4, 4, 0);
        assert(test2 == 0); 
        test2 = Bitpack_getu(test2, 4, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == 0);
}

void new_u_test_4() {
        uint64_t test_word = 1;
        uint64_t test2 = Bitpack_newu(test_word, 3, 1, 2);
        assert(test2 == 5); 
        test2 = Bitpack_getu(test2, 4, 0);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == 5);
}

void new_u_all() {
        uint64_t test_word = ~0;
        printf("made it\n"); 
        uint64_t test2 = Bitpack_newu(test_word, 32, 32, 2);
        printf("Test2 is %lu\n", test2);
        //assert(test2 == 11); 
}

void new_u_test_law_1() {
        uint64_t word = 0x3f4;
        uint64_t w = 4;
        uint64_t lsb = 0;
        uint64_t val = 8;
        printf("testing spec law 1\n");
        uint64_t resultword = Bitpack_newu(word, w, lsb, val);
        assert(Bitpack_getu(resultword, w, lsb) == val);
}

void new_u_test_law_2() {
        uint64_t word = 0x3f4;
        uint64_t w = 4;
        uint64_t lsb = 0;
        uint64_t val = 8;
        uint64_t w2 = 6;
        uint64_t lsb2 = w + lsb;
        printf("testing spec law 2\n");
        assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w2, lsb2) == Bitpack_getu(word, w2, lsb2));
}

void new_s_test_1() {
        int64_t test_word = ~0;
        assert(test_word == (int64_t)0xFFFFFFFFFFFFFFFF);
        // printf("asserted here\n");
        // ~0 is a short way to write all 1s in as many bits as a type can fit
        // 8 is 01000 in 2s complement
        int64_t test2 = Bitpack_news(test_word, 5, 4, 8);
        printf(" test 2 is: %lu\n", test2);
        assert(test2 == (int64_t)0xFFFFFFFFFFFFFE8F);
        // printf("asserted here\n");
        test2 = Bitpack_gets(test2, 5, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == 8);
        assert(test2 == (int64_t)0x8);
}
void new_s_test_2() {
        int64_t test_word = 0;
        int64_t test2 = Bitpack_news(test_word, 4, 4, -3);
        // assert(test2 = 0x0000000000000050); // when i comment this, 328 passes otherwise, it does not
        test2 = Bitpack_gets(test2, 4, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == -3);
}

void new_s_test_3() {
        int64_t test_word = ~0;
        // ~0 is a short way to write all 1s in as many bits as a type can fit
        // 1000 in binary which is -8 in 2s complement
        // printf("made it to line 263\n"); 
        int64_t test2 = Bitpack_news(test_word, 5, 4, -7);
        assert(test2 == (int64_t)0xFFFFFFFFFFFFFF9F); 
        test2 = Bitpack_gets(test2, 5, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == -7);
        // assert(test2 == (int64_t)0xFFFFFFFFFFFFFFF9);
}

void new_s_test_4() {
        int64_t test_word = ~0;
        int64_t test2 = Bitpack_news(test_word, 4, 4, -5);
        test2 = Bitpack_gets(test2, 4, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == -5);
}
void new_s_test_4_eidted() {
        int64_t test_word = 0x00000000000000AA;
        int64_t test2 = Bitpack_news(test_word, 4, 0, -4);
        test2 = Bitpack_gets(test2, 3, 0);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == -4);
}
void new_s_test_4_eidted_2() {
        int64_t test_word = 0x00000000000000AA;
        int64_t test2 = Bitpack_news(test_word, 4, 0, -1);
        test2 = Bitpack_gets(test2, 4, 0);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == -1);
}

void new_s_test_4_eidted_3() {
        int64_t test_word = 0x00000000000000AA;
        int64_t test2 = Bitpack_news(test_word, 4, 0, -1);
        test2 = Bitpack_gets(test2, 5, 0);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == 15); // why
}

void new_s_test_5() {
        int64_t test_word = 0;
        int64_t test2 = Bitpack_news(test_word, 4, 4, 3);
        test2 = Bitpack_gets(test2, 4, 4);
        printf("was able to write and grab value: %li\n", test2);
        assert(test2 == 3);
}

void new_s_all() {
        int64_t test_word = ~0;
        printf("made it\n"); 
        int64_t test2 = Bitpack_news(test_word, 32, 32, 2);
        printf("Test2 is %lu\n", test2);
}

void new_s_test_law_1() {
        int64_t word = 0x3f4;
        int64_t w = 5;
        int64_t lsb = 0;
        int64_t val = 8;
        printf("testing spec law 1\n");
        int64_t resultword = Bitpack_news(word, w, lsb, val);
        assert(Bitpack_gets(resultword, w, lsb) == val);
}

void new_s_test_law_2() {
        uint64_t word = 0x3f4;
        uint64_t w = 5;
        uint64_t lsb = 0;
        uint64_t val = 8;
        uint64_t w2 = 6;
        uint64_t lsb2 = w + lsb;
        printf("testing spec law 2\n");
        assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w2, lsb2) == Bitpack_getu(word, w2, lsb2));
}


// more cases to test: width 1, lsb 0: should get first bit in the word
// 

int main(int argc, char *argv[])
{
        (void) argc;
        (void) argv;
        test_fitu_true();
        test_fitu_false();
        test_fitu_15_test();
        test_fitu_1_test();
        test_fitu_0_width_test();
        test_fitu_no_width_true();
        test_fitu_no_width_false();
        printf("Fit_unsigned tests are all above this line\n");
        test_fit_signed_0_1();
        test_fits_negative_false_0_0();
        test_fit_signed_0_neg_1();
        test_fit_signed_1_0();
        test_fits_positive_true();
        test_fits_positive_false();
        test_fits_negative_true();
        test_fits_negative_false();
        test_fits_negative_false_1_bit();
        test_fits_negative_false_2_bit();
        printf("Fit_signed tests are all above this line\n");
        get_u_test_15();
        get_u_test_7(); 
        get_u_test_spec();
        get_u_test_edge();
        get_u_test_edge_2();
        get_u_test_edge_3();
        get_u_test_edge_4();
        get_u_test_edge_5();
        get_u_test_spec_edited();
        get_u_test_width_64();
        printf("Get_unsigned tests are all above this line\n");
        get_s_test_spec();
        get_s_test_spec_1();
        get_s_test_spec_2(); 
        get_s_test_spec_3();
        get_s_test_spec_4();
        get_s_test_width_64();
        get_u_s_test_comparison();
        get_u_s_test_comparison_positive();
        printf("Get_signed tests are all above this line\n");

        new_u_all();
        new_u_test_1();
        new_u_test_1_width_5();
        new_u_test_2();
        new_u_test_3();
        new_u_test_4();
   
        new_u_test_law_1();
        new_u_test_law_2();
        printf("New_unsigned tests are all above this line\n");
        new_s_test_1();
        new_s_test_2();
        new_s_test_3();
        new_s_test_4();
        new_s_test_4_eidted();
        new_s_test_4_eidted_2();
        new_s_test_4_eidted_3(); // this does not work
        new_s_test_5();
        new_s_all();
        new_s_test_law_1();
        new_s_test_law_2();
        printf("New_signed tests are all above this line\n");
        return 0;
}




/*

According to the spec:

Bitpack_getu(0x3f4, 6, 2) == 61
111101
Bitpack_gets(0x3f4, 6, 2) == -3
111101


Some other helpful info for making tests with:
0x3f4 is
1111110100


61 in 64 bit 2s comp
0000000000000000000000000000000000000000000000000000000000111101

-3
1111111111111111111111111111111111111111111111111111111111111101

15 in 64 bit 2s comp
0000000000000000000000000000000000000000000000000000000000001111

1101


1012
1111110100


57
111001


*/