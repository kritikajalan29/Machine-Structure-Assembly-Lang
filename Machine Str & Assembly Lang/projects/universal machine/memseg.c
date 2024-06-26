//*****************************************************************************
//
//                          memseg.c
//
//
//     Author: Kritika Jalan and Ishaan Arora
//     
//     Purpose: Emulates main memory in a um machine. Stores memory segments as
//              Hanson sequences of c arrays. Deals with creation/storage of 
//              segmented memory. 
//
//     Tufts University, Fall 2022
//     CS 40: HW6 UM
//
//     Date: 11/22/2022
//
//*****************************************************************************

#include "memseg.h"

static Seq_T unmapped_id;
static Seq_T mapped_id;
static Seq_T sizes;

static uint32_t id = 0;

/*
 * Function: initialize_memory
 * Purpose: initializes memory id and size sequences for use by memory segments
 * Input: unsigned int size -> initial size of program
 * Returns: N/A
 * Expectations: size is correctly passed in
 */
void initialize_memory(unsigned int size)
{
        /* allocates space for global sequences */
        unmapped_id = Seq_new(10);
        mapped_id = Seq_new(10);
        sizes = Seq_new(10);

        /* Adds 0 to mapped_id and its corresponding size to sizes */
        Seq_addhi(mapped_id, (void *)(uintptr_t)(0));
        size = size / 4;
        Seq_addhi(sizes, (void *)(uintptr_t)size);
}

/*
 * Function: new_segment
 * Purpose: creates a new memory segment with a new id
 * Input: unsigned int instr_size -> size of instruction words in 1 segment
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: instruction array is not NULL
 */
void new_segment(Seq_T segments, unsigned int instr_size)
{
        /* allocates space for C Arrays & initialises to 0 */
        uint32_t *instruction;
        instruction = calloc(instr_size, sizeof(uint32_t));
        assert(instruction != NULL);

        /* Pushes instruction and its size to appropriate sequences */
        Seq_addhi(segments, instruction);
        Seq_addhi(sizes, (void *)(uintptr_t)instr_size);
}

/*
 * Function: renew_segment
 * Purpose: creates a new memory segment with a recycled id
 * Input: unsigned int instr_size -> size of instruction words in 1 segment
 *        Seq_T segments -> the sequence of memory segments
 *        uint32_t curr_id -> recycled id
 * Returns: N/A
 * Expectations: instruction array is not NULL
 */
void renew_segment(Seq_T segments, unsigned int instr_size, uint32_t curr_id)
{
        /* allocates space for C Arrays & initialises to 0 */
        uint32_t *instruction;
        instruction = calloc(instr_size, sizeof(uint32_t));
        assert(instruction != NULL);

        /* Puts instruction and its size to appropriate sequences 
                                                at appropriate indices */
        Seq_put(segments, curr_id, instruction);
        Seq_put(sizes, curr_id, (void *)(uintptr_t)instr_size);
}

/*
 * Function: map_segment
 * Purpose: helper function to map a memory segment
 * Input: unsigned int instr_size -> size of instruction words in 1 segment
 *        Seq_T segments -> the sequence of memory segments
 * Returns: uint32_t curr_id -> returns the id of the mapped segment
 * Expectations: creates a new segment correctly
 */
uint32_t map_segment(Seq_T segments, unsigned int instr_size)
{
        uint32_t curr_id;
        /* checks if there are any unmapped ids */
        if (Seq_length(unmapped_id) > 0) {
                /* recycles id */
                curr_id = (uint32_t)(uintptr_t)Seq_remhi(unmapped_id);
                Seq_addhi(mapped_id, (void *)(uintptr_t)curr_id);
                renew_segment(segments, instr_size, curr_id);
        } else {
                /* otherwise increments global id allocator */
                id = id + 1;
                Seq_addhi(mapped_id, (void *)(uintptr_t)id);
                curr_id = id;
                new_segment(segments, instr_size);
        }
        
        return curr_id;
       
}

/*
 * Function: unmap_segment
 * Purpose: helper function to unmap a memory segment
 * Input: uint32_t id -> id of the segment to be unmapped
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: frees a segment correctly
 */
void unmap_segment(Seq_T segments, uint32_t id)
{
        /* gets C Array at given id */
        uint32_t *curr_instruction;
        curr_instruction = (uint32_t *) Seq_get(segments, id);
        FREE(curr_instruction);
        
        Seq_put(segments, id, NULL);
        

        /* keeps track of unmapped ids */
        Seq_addhi(unmapped_id, (void *)(uintptr_t)id);

}

/*
 * Function: load_program_segment
 * Purpose: helper function to load a memory segment into segment 0
 * Input: uint32_t id -> id of the segment to be loaded into segment 0
 *        Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: duplicated segment is not NULL
 */
void load_program_segment(Seq_T segments, uint32_t id)
{
        /* gets segment to be executed */
        uint32_t *original_segment;
        original_segment = Seq_get(segments, id);

        /* gets its size */
        unsigned int original_size;
        original_size = (uint32_t)(uintptr_t)Seq_get(sizes, id);

        /* creates duplicate and allocates space for it */
        uint32_t *duplicate;
        duplicate = calloc(original_size, sizeof(uint32_t));
        assert(duplicate != NULL);

        /* deep copy */
        for (unsigned int i = 0; i < original_size; i++) {
                duplicate[i] = original_segment[i];
        }

        /* frees old 0 segment */
        free(Seq_get(segments, 0));

        /* puts new 0 segment and size into appropriate sequences */
        Seq_put(segments, 0, duplicate);
        Seq_put(sizes, 0, (void *)(uintptr_t)original_size);
}

/*
 * Function: halt_helper
 * Purpose: helper function to free all memory segments and all other sequences
 * Input: Seq_T segments -> the sequence of memory segments
 * Returns: N/A
 * Expectations: no memory leaks
 */
void halt_helper(Seq_T segments)
{
        /* frees memory */
        assert(segments != NULL);

        int length = Seq_length(segments);

        for (int i = 0; i < length; i++) {
                uint32_t *curr_instruction;
                curr_instruction = Seq_get(segments, i);
                if (curr_instruction != NULL) {
                        FREE(curr_instruction);
                }
                
        }
//kjia

        Seq_free(&segments);
        Seq_free(&sizes);
        Seq_free(&mapped_id);
        Seq_free(&unmapped_id);
}
