//*****************************************************************************
//
//                          unpack.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: Unpacks the 32-bit instruction into opcode, registers and value
//              (in the case of Load Value). Uses the bitpack implememtation to
//              do so.
//
//     Tufts University, Fall 2022
//     CS 40: HW6 UM
//
//     Date: 11/22/2022
//
//*****************************************************************************

#include "unpack.h"


/*
 * Function: opcode_unpack
 * Purpose: unpacks the opcode with LSB 28 and width 4
 * Input: uint32_t instruction
 * Returns: unsigned int opcode 
 * Expectation: opcode should be between 0 and 13
 */
unsigned int opcode_unpack(uint32_t word)
{
        return (Bitpack_getu(word, 4, 28));
}

/*
 * Function: ra_unpack
 * Purpose: unpacks the ra register with LSB 6 and width 3
 * Input: uint32_t instruction
 * Returns: unsigned ra 
 * Expectations: ra number should be between 0 and 7
 */
unsigned int ra_unpack(uint32_t word)
{
       return (Bitpack_getu(word, 3, 6));
}

/*
 * Function: rb_unpack
 * Purpose: unpacks the rb register with LSB 3 and width 3
 * Input: uint32_t instruction
 * Returns: unsigned rb 
 * Expectations: rb number should be between 0 and 7
 */
unsigned int rb_unpack(uint32_t word)
{
        return (Bitpack_getu(word, 3, 3));
}

/*
 * Function: rc_unpack
 * Purpose: unpacks the rc register with LSB 0 and width 3
 * Input: uint32_t instruction
 * Returns: unsigned rc
 * Expectations: rc number should be between 0 and 7
 */
unsigned int rc_unpack(uint32_t word)
{
        return (Bitpack_getu(word, 3, 0));
}

/*
 * Function: lv_ra_unpack
 * Purpose: unpacks the ra register for the LV opcode with LSB 25 and width 3
 * Input: uint32_t instruction
 * Returns: unsigned lv_ra
 * Expectations: ra number should be between 0 and 7
 */
unsigned int lv_ra_unpack(uint32_t word)
{
        return (Bitpack_getu(word, 3, 25));
}

/*
 * Function: lv_value_unpack
 * Purpose: unpacks the value loaded into main memory with LSB 0 and width 25
 * Input: uint32_t instruction
 * Returns: unsigned lv_value 
 * Expectations: N/A
 */
unsigned int lv_value_unpack(uint32_t word)
{
        return (Bitpack_getu(word, 25, 0));
}


