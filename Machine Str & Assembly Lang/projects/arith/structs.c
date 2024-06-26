//*****************************************************************************
//
//                          structs.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: Source file for definitions of key structs used in compression
//              and decompression algorithms. Contains a bound checking function
//              that is implemented by several modules.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 10/28/2022
//
//*****************************************************************************
#include "structs.h"

/*
    Function: bound_check
    Purpose: Checks whether given value is in bounds 
    Inputs: double value → value to be checked
            double bound → value to be checked against
    Returns: double value → the value with updated bounds
    Expectation: 
*/
double bound_check(double value, double bound)
{
        /* checks if values passed in is rgb or not */
        if (bound == DENOMINATOR) {
                /* handles rgb values */
                if (value > bound) {
                    value = bound;
                }
                else if (value < 0) {
                        value = 0;
                }
        }
        else {
                /* handles other values */
                if (value > bound) {
                        value = bound;
                }
                else if (value < (-1.0 * bound)) {
                        value = (-1.0 * bound);
                }

        }

        return value;
}