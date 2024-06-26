//*****************************************************************************
//
//                          bitpack.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: This module contains all necessary functions for bitpacking and
//              unpacking, including functions to check whether an integer can
//              be represented by a specific number of bits. It deals with 
//              right and left shifts in order to retrieve a field from a packed
//              word and also update field values in words.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "bitpack.h"
#include "except.h"

/* Helper Functions declaration*/
uint64_t shift_left(uint64_t word, unsigned int amount);
uint64_t shift_rightu(uint64_t word, unsigned int amount);
int64_t shift_rights(int64_t word, unsigned int amount);

Except_T Bitpack_Overflow = { "Overflow packing bits" };

const unsigned int max = 64;

/*
    Function: Bitpack_fitsu
    Purpose: Checks whether or not an unsigned integer n can be represented 
             in width bits
    Inputs: uint64_t n - unsigned integer to be checked against width bits
            unsigned width - the number of bits to represent n
    Returns: true if n can be represented in width bits, false if not
    Expectation: width has to be non-negative.

*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        unsigned int lower_range = 0;
        unsigned int upper_range = 1 << width;
        upper_range--; 

        /* Checking if value n is in range */
        if (n >= lower_range && n <= upper_range) {
                return true;
        }

        return false;
}

/*
    Function: Bitpack_fitss
    Purpose: Checks whether or not a signed integer n can be represented 
             in width bits
    Inputs: int64_t n - signed integer to be checked against width bits
            unsigned width - the number of bits to represent n
    Returns: true if n can be represented in width bits, false if not
    Expectation: width has to be non-negative.

*/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        int lower_range = 1 << (width - 1);
        lower_range = (-1) * lower_range;

        int upper_range = 1 << (width - 1);
        upper_range--;

        /* Checking if value n is in range */
        if (n >= lower_range && n <= upper_range) {
                return true;
        }

        return false;
}

/*
    Function: Bitpack_getu
    Purpose: Given a 64-bit word, it returns the unsigned value of the word of 
             width bits with Least Signfiicant Bit at lsb
    Inputs: uint64_t word - given 64-bit word
            unsigned width - width of the word to be extracted
            unsigned lsb - lsb of the word to be extracted
    Returns: unsigned integer value of extracted word of width bits 
             starting at lsb. Returns 0 if width is 0.
    Expectation: width has to be less than or equal tomax (64). 
                 width + lsb has to be at most equal to max (64).

*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= max);
        assert ((width + lsb) <= max);

        /* If width = 0, we return the value 0 */
        if (width == 0) {
                return 0;
        }
        
        word = shift_left(word, (max - (width + lsb)));
        word = shift_rightu(word, (max - width));

        return word;
}

/*
    Function: Bitpack_gets
    Purpose: Given a 64-bit word, it returns the signed value of the word of 
             width bits with Least Signfiicant Bit at lsb
    Inputs: uint64_t word - given 64-bit word
            unsigned width - width of the word to be extracted
            unsigned lsb - lsb of the word to be extracted
    Returns: signed integer value of extracted word of width bits 
             starting at lsb. Returns 0 if width is 0.
    Expectation: width must be less than or equal to max (64). 
                 width + lsb must be at most equal to max (64).

*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= max);
        assert ((width + lsb) <= max);

        int64_t s = word;

        /* If width = 0, we return the value 0 */
        if (width == 0) {
                return 0;
        }

        s = shift_left(s, (max - (width + lsb)));
        s = shift_rights(s, (max - width));
        
        return s;

        
}

/*
    Function: Bitpack_newu
    Purpose: Given a 64-bit word, and a value, the function replaces the 
    specified field with the new value and returns the updated word. 
    Inputs: uint64_t word - given 64-bit word
            unsigned width - width of the word to be extracted
            unsigned lsb - lsb of the word to be extracted
            uint64_t value - the new value that must replace the old value to
                             give the updated word
    Returns: uint64_t word updated with passed in value.
    Expectation: width must be less than or equal to max (64). 
                 width + lsb must be at most equal to max (64).

*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                                uint64_t value)
{
        assert(width <= max);
        assert ((width + lsb) <= max);

        if (Bitpack_fitsu(value, width) == false) {
                RAISE(Bitpack_Overflow); 
        }

        /* mask 1 puts 1s to the left of the value */
        uint64_t mask1 = ~0;
        mask1 = shift_left(mask1, (width + lsb));
        
        /* mask 2 puts 1s to the right of the value */
        uint64_t mask2 = ~0;
        mask2 = shift_rightu(mask2, (max - lsb));

        /*final_mask has 1s on both sides of the values and 0s where the value
        should be */
        uint64_t final_mask = mask1 | mask2;
        uint64_t new_word = word & final_mask;
        
        value = shift_left(value, lsb);

        /* updates the word with the new value */
        new_word = new_word | value;

        return new_word;
}

/*
    Function: Bitpack_news
    Purpose: Given a 64-bit word, and a signed value, the function replaces the 
    specified field with the new value and returns the updated word. 
    Inputs: uint64_t word - given 64-bit word
            unsigned width - width of the word to be extracted
            unsigned lsb - lsb of the word to be extracted
            int64_t value - the new signed value that must replace the old value
                             to give the updated word
    Returns: uint64_t word updated with passed in value.
    Expectation: width must be less than or equal to max (64). 
                 width + lsb must be at most equal to max (64).

*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                                int64_t value)
{
        assert(width <= max);
        assert ((width + lsb) <= max);

        if (Bitpack_fitss(value, width) == false) {
                RAISE(Bitpack_Overflow); 
        }

        /* mask 1 puts 1s to the left of the value */
        uint64_t mask1 = ~0;
        mask1 = shift_left(mask1, (width + lsb));

        /* mask 2 puts 1s to the right of the value */
        uint64_t mask2 = ~0;
        mask2 = shift_rightu(mask2, (max - lsb));

        /*final_mask has 1s on both sides of the values and 0s where the value
        should be */
        uint64_t final_mask = mask1 | mask2;
        uint64_t new_word = (word & final_mask);

        value = shift_left(value, lsb);
        uint64_t new_value = value;

        /* accounting for signed ints */
        new_value = shift_left(new_value, (max - (lsb + width)));
        new_value = shift_rightu(new_value, max - (lsb + width));
        
        /* updates the word with the new value */
        new_word = new_word | new_value;

        return new_word;
}

/*
    Function: shift_left
    Purpose: Given a 64-bit word, and the magnitude of the shift, the function 
    shifts the given word to the left by the 'amount' (magnitude).
    Inputs: uint64_t word - given 64-bit word
            int amount - the magnitude of the shift
    Returns: uint64_t word updated with the shift.
    Expectation: width must be less than or equal to max (64). 
                 width + lsb must be at most equal to max (64)
                 (gets asserted in functions that call shift_left).

*/
uint64_t shift_left(uint64_t word, unsigned int amount)
{
        uint64_t new_word;
        
        if (amount == max) {                
                new_word = 0;
                return new_word;
        }
        
        new_word = word << amount;
        
        return new_word;
}

/*
    Function: shift_rightu
    Purpose: Given a 64-bit word, and the magnitude of the shift, the function 
    shifts the given unsigned word to the right by the 'amount' (magnitude).
    Inputs: uint64_t word - given 64-bit word
            int amount - the magnitude of the shift
    Returns: uint64_t unsigned word updated with the shift.
    Expectation: width must be less than or equal to max (64). 
                 width + lsb must be at most equal to max (64)
                 (gets asserted in functions that call shift_rightu).

*/
uint64_t shift_rightu(uint64_t word, unsigned int amount)
{
        uint64_t new_word;
        
        if (amount == max) {                
                new_word = 0;
                return new_word;
        }
        
        new_word = word >> amount;
        
        return new_word;
}

/*
    Function: shift_rights
    Purpose: Given a 64-bit word, and the magnitude of the shift, the function 
    shifts the given signed word to the right by the 'amount' (magnitude).
    Inputs: uint64_t word - given 64-bit signed word
            int amount - the magnitude of the shift
    Returns: int64_t signed word updated with the shift.
    Expectation: width must be less than or equal to max (64). 
                 width + lsb must be at most equal to max (64)
                 (gets asserted in functions that call shift_rights).

*/
int64_t shift_rights(int64_t word, unsigned int amount)
{
        int64_t new_word;
        
        if (amount == max) {                
                new_word = ~0;
                return new_word;
        }
        
        new_word = word >> amount;
        
        return new_word;
}
                

