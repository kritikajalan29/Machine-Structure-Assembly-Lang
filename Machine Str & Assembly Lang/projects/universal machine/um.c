//*****************************************************************************
//
//                          um.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: Driver module. Opens the file and reads in the program file.
//              Calls the unpack module to unpack the words and finally the 
//              operations module to execute the instructions.
//
//     Tufts University, Fall 2022
//     CS 40: HW4 arith
//
//     Date: 11/22/2022
//
//*****************************************************************************

#include "um.h"


/*
 * Function: main
 * Purpose: calls functions to open and read a file
 * Input: int argc, char *argv[]
 * Returns: int [0 if program runs successfully]
 * Expectations: File stream should not be null
 */
int main(int argc, char *argv[])
{
        /* Usage message */
        if (argc != 2) {
                fprintf(stderr, "usage: ./um some_program.um %s",
                 "testinput.txt > output.txt\n");
                exit(1);
        }
        assert(argc == 2);

        /* Creates the original 0 segment */
        Seq_T segments = Seq_new(50); // emulating main memory
        
        FILE *file = open_file(argv[1], "r");

        /* getting the size of the program file */
        struct stat st;
        stat(argv[1], &st);
        int size = st.st_size;

        read_program(segments, file, size);
        
        return 0;
}
/*
 * Function: open_file
 * Purpose: opens a file for reading
 * Input: char *fname -> name of file
 *        char *mode -> mode of opening the file
 * Returns: FILE * -> returns the file stream
 * Expectations: File stream should not be null
 */
FILE *open_file(char *fname, char *mode) 
{
        FILE *fp = fopen(fname, mode);
        assert(fp != NULL);
        return fp;
}


/*
 * Function: read_program
 * Purpose: reads in a .um program one byte at a time in big endian order and
 *          stores it in an array of uint32_t s
 * Input: Seq_T segments -> Sequence of memory segments to be populated
 *        FILE *fp -> pointer to program file that needs to be read in
 *        int size -> size of program file in bytes
 * Returns: N/A 
 * Expectation: file pointer is not null
 */
void read_program(Seq_T segments, FILE *fp, unsigned int size)
{
        int byte;
        /* allocates space for C Arrays & initialises to 0 */     
        uint32_t *instruction;
        instruction = calloc(size, sizeof(uint32_t));
        assert(instruction != NULL);

        initialize_memory(size);

        /* Reads in instructions from file in big-endian byte order */
        uint32_t word;
        for (unsigned int j = 0; j < size / 4; j++) {
                for (int i = 24; i >= 0; i = i - 8) {
                        byte = fgetc(fp);
                        word = (uint32_t) Bitpack_newu(word, 8, i, byte);
                }
                instruction[j] = word;
        }

        /* Closes file */
        int check_close = fclose(fp); 
        assert(check_close == 0);
        
        Seq_addhi(segments, instruction);
        unpack_word(segments);
        
}

/*
 * Function: unpack_word
 * Purpose: calls the unpack module to unpack the 32 bit instruction into 
 *          opcode, registers and value (if needed). It then calls the 
 *          operations module to carry out the instructions.
 * Input: Seq_T segments -> Sequence of memory segments
 * Returns: N/A
 * Expectations: Segment 0 is populated with a program
 */
void unpack_word (Seq_T segments)
{
        uint32_t program_counter = 0;
        /* gets instruction from segment */
        uint32_t *instruction;
        instruction = Seq_get(segments, 0);
        unsigned int opcode, ra, rb, rc, lv_ra, lv_value;
       
       /* Runs loop until halt() is called */
        do {
                opcode = opcode_unpack(instruction[program_counter]);

                /* Calls different unpack functions for load value */
                if (opcode == 13) {
                        lv_ra = lv_ra_unpack(instruction[program_counter]);
                        lv_value = lv_value_unpack(instruction
                                                        [program_counter]);

                        execute_lv(lv_ra, lv_value);
                        
                        program_counter++;
                }
                /* Calls regular unpack functions for all other operations */
                else {
                        ra = ra_unpack(instruction[program_counter]);
                        rb = rb_unpack(instruction[program_counter]);
                        rc = rc_unpack(instruction[program_counter]);

                        execute_operation(opcode, ra, rb, rc, segments, 
                                        &program_counter);
                        /* Increments program counter only if 
                                                load program  is not called */
                        if (opcode != 12) {
                                program_counter++;
                        } else {
                                /* gets new set of instructions
                                                when load program is called */
                                instruction = Seq_get(segments, 0);
                        }

                }

        } while (opcode != 7);
                
}