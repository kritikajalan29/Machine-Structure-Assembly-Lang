//*****************************************************************************
//
//                          operations.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: Has a switch case to handle the 14 UM operations. Calls 
//              necessary functions and helper functions to execute the um
//              instructions. Memory operations call the memory segment module
//              for manipulation of memory 
//
//     Tufts University, Fall 2022
//     CS 40: HW6 UM
//
//     Date: 11/22/2022
//
//*****************************************************************************

#include "operations.h"

static uint32_t registers[8];
static const long LIMIT = 4294967296; // 2^32

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

/* Static inline declarations of helper operation functions */

static inline void conditional_move(unsigned int ra, unsigned int rb, 
                                                            unsigned int rc);
static inline void add(unsigned int ra, unsigned int rb, unsigned int rc);
static inline void multiply(unsigned int ra, unsigned int rb, unsigned int rc);
static inline void divide(unsigned int ra, unsigned int rb, unsigned int rc);
static inline void nand(unsigned int ra, unsigned int rb, unsigned int rc);
static inline void output(unsigned int rc);
static inline void input(unsigned int rc);
static inline void halt(Seq_T segments);
static inline void seg_load(unsigned int ra, unsigned int rb, unsigned int rc, 
                                                            Seq_T segments);
static inline void seg_store(unsigned int ra, unsigned int rb, unsigned int rc,
                                                         Seq_T segments);
static inline void map(Seq_T segments, unsigned int rb, unsigned int rc);
static inline void unmap(Seq_T segments, unsigned int rc);
static inline void load_program(Seq_T segments, unsigned int rb, 
                                                unsigned int rc, uint32_t *pc);

/*
 * Function: execute_operation
 * Purpose: a switch case that calls helper functions to execute instructions
 *          based on the opcode
 * Input: unsigned int opcode -> the code of instruction
 *        unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 *        Seq_T segments -> the sequence of memory segments
 *        uint32_t *pc -> a pointer to the program counter
 * Returns: N/A
 * Expectations: Range of opcode is between 0 and 13
 */
void execute_operation(unsigned int opcode, unsigned int ra, unsigned int rb, 
                       unsigned int rc, Seq_T segments, uint32_t *pc)
{
        switch (opcode) {
                case CMOV:
                        conditional_move(ra, rb, rc);
                        break;
                case SLOAD:
                        seg_load(ra, rb, rc, segments);
                        break;
                case SSTORE:
                        seg_store(ra, rb, rc, segments);
                        break;
                case ADD:
                        add(ra, rb, rc);
                        break;
                case MUL:
                        multiply(ra, rb, rc);
                        break;
                case DIV:
                        divide(ra, rb, rc);
                        break;
                case NAND:
                        nand(ra, rb, rc);
                        break;
                case HALT:
                        halt(segments);
                        break;
                case MAP:
                        map(segments, rb, rc);
                        break;
                case UNMAP:
                        unmap(segments, rc);
                        break;
                case OUT:
                        output(rc);
                        break;
                case IN:
                        input(rc);
                        break;
                case LOADP:
                        load_program(segments, rb, rc, pc);
                        break;

        }
}

/*
 * Function: execute_lv
 * Purpose: Executes the Load Value operation 
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int value -> the value to be loaded in register ra
 * Returns: N/A
 * Expectations: Value is an unsigned int
 */
void execute_lv(unsigned int ra, unsigned int value)
{
        /* puts given value in register ra */
        registers[ra] = value;
}

/*
 * Function: conditional_move
 * Purpose: Executes the Conditioonal Move operation -- checks is value in 
 *          rc = 0. If not, it copies the value from register rb to register ra
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: value at rc is not NULL
 */
static inline void conditional_move(unsigned int ra, unsigned int rb, 
                                                        unsigned int rc) 
{
        /* checks if value in rc = 0 and accordingly executes instruction*/
        if (registers[rc] != 0) {
                registers[ra] = registers[rb];
        }

}

/*
 * Function: seg_load
 * Purpose: Executes the Segmented Load operation -- extracts a word from a 
 *          given segment and word index. Populates register ra with the 
 *          extracted 32-bit word. 
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: word at $m[$r[B]][$r[C]] exists
 */
static inline void seg_load(unsigned int ra, unsigned int rb, unsigned int rc, 
                                                                Seq_T segments)
{
        uint32_t *curr;
        /* gets segment at id stored in register rb*/
        curr = Seq_get(segments, registers[rb]);
        /* gets word at index stored in register rc*/
        uint32_t extracted_word = curr[registers[rc]];
        /* stores the extracted word in register ra*/
        registers[ra] = extracted_word;

}

/*
 * Function: seg_store
 * Purpose: Executes the Segmented Store operation -- takes a 32-bit word from 
 *          register rc and stores it in index $m[$r[A]][$r[B]].
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: word at [$r[C]] must exist and segment $m[$r[A]] must exist 
 *               as well
 */
static inline void seg_store(unsigned int ra, unsigned int rb, unsigned int rc,
               Seq_T segments)
{
        uint32_t *curr;
        /* gets word to be stored from register rc*/
        uint32_t store_word = registers[rc];
        /* gets segment at id stored in register ra*/
        curr = Seq_get(segments, registers[ra]);
        /* puts word at index stored in register rb*/
        curr[registers[rb]] = store_word;

}

/*
 * Function: add
 * Purpose: Executes the Addition operation -- adds the values in registers
 *          rb and rc and stores the sum in register ra
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: registers rb and rc have some value
 */
static inline void add(unsigned int ra, unsigned int rb, unsigned int rc)
{
       unsigned int sum = registers[rb] + registers[rc];
       /* LIMIT = 2^32 */
       sum =  sum % LIMIT;
       registers[ra] = sum;

}

/*
 * Function: multiply
 * Purpose: Executes the Multiplication operation -- multiplies the values in 
 *          registers rb and rc and stores the product in register ra
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: registers rb and rc have some value
 */
static inline void multiply(unsigned int ra, unsigned int rb, unsigned int rc)
{
       unsigned int product = (registers[rb]) * (registers[rc]);
       /* LIMIT = 2^32 */
       product = product % LIMIT;
       registers[ra] = product;

}

/*
 * Function: divide
 * Purpose: Executes the Division operation -- divides the values in 
 *          register rb with the value in register rc and stores the quotient
 *          in register ra.
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: registers rb and rc have some value
 */
static inline void divide(unsigned int ra, unsigned int rb, unsigned int rc)
{
       unsigned int quotient = (registers[rb]) / (registers[rc]);
       registers[ra] = quotient;

}

/*
 * Function: nand
 * Purpose: Executes the Bitwise NAND operation -- ands the values in 
 *          register rb with the value in register rc . It nots the result and
 *          stores the quotient in register ra.
 * Input: unsigned int ra -> the register number used as ra
 *        unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: registers rb and rc have some value
 */
static inline void nand(unsigned int ra, unsigned int rb, unsigned int rc)
{
        registers[ra] = ~(registers[rb] & registers[rc]);
}

/*
 * Function: output
 * Purpose: Executes the output operation -- prints the values in rc to stdout.
 * Input: unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: register rc has some value
 */
static inline void output(unsigned int rc)
{
        /* values not in range (0-255), um allowed to fail */
        putchar(registers[rc]);
}

/*
 * Function: input
 * Purpose: Executes the input operation -- takes a single char input from stdin
 *          and stores it in register rc
 * Input: unsigned int rc -> the register number used as rc
 * Returns: N/A
 * Expectations: input is within range (0-255)
 */
static inline void input(unsigned int rc)
{
        int input;
        input = getchar();
        if (input == EOF) {
                /* If EOF character then rc populated with a uint32_t of ones */
                uint32_t ones = ~0;
                registers[rc] = ones;
        } else {
                assert(input >= 0 && input <= 255);
                registers[rc] = input;
        }        
         
}

/*
 * Function: halt
 * Purpose: Frees all dynamically allocated memory and stops computation
 * Input: N/A
 * Returns: N/A
 * Expectations: Sequences are non-null so they can be freed
 */
static inline void halt(Seq_T segments)
{
        halt_helper(segments);
        exit(EXIT_SUCCESS);
}

/*
 * Function: map
 * Purpose: Executes the Map Segment operation -- creates a new memory segment/
 *          reuses an old unmapped memory segment
 * Input: unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: register rc provides the size of the new segment to be mapped
 */
static inline void map(Seq_T segments,  unsigned int rb, unsigned int rc)
{
        int instr_size = registers[rc];
        /* Calls helper function in memory segment module to map a segment */
        uint32_t id_value = map_segment(segments, instr_size);
        registers[rb] = id_value;

}

/*
 * Function: unmap
 * Purpose: Executes the Unmap Segment operation -- frees a memory segment
 * Input: unsigned int rc -> the register number used as rc
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: register rb provides the id of the segment to be unmapped
 */
static inline void unmap(Seq_T segments, unsigned int rc)
{
        uint32_t id = registers[rc];
        /* Calls helper function in memory segment module to unmap a segment */
        unmap_segment(segments, id);

}

/*
 * Function: load_program
 * Purpose: Executes the Load Program operation -- loads a new segment of 
 *          instructions into segment 0
 * Input: unsigned int rb -> the register number used as rb
 *        unsigned int rc -> the register number used as rc
 *        Seq_T segments -> the sequence of memory segments
 *        uint32_t *pc --> pointer to program counter
 * Returns: N/A
 * Expectations: register rb provides the id of the segment to be loaded
 */
static inline void load_program(Seq_T segments, unsigned int rb, 
                                                unsigned int rc, uint32_t *pc)
{
        
        uint32_t id = registers[rb];
        /* Edge case if segment 0 is being loaded */
        if (id != 0) {
                load_program_segment(segments, id);
        }
        *pc = registers[rc];


}
