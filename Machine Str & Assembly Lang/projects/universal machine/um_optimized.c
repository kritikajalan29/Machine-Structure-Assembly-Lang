#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include "mem.h"
#include <inttypes.h>

static uint32_t registers[8];
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

typedef struct segments {
        int curr_size;
        uint32_t *arr[];
} *segments;

typedef struct unmapped {
        int curr_size;
        uint32_t id[];
} *unmapped;

typedef struct size_struct {
        int curr_size;
        uint32_t sizes[];
} *size_struct;

// static struct unmapped *unmapped_ids;
// static struct size_struct *sizes;
static uint32_t id = 0;


//*****************************************************************************
//                             um.c
//*****************************************************************************

int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "usage: ./um some_program.um %s",
                 "testinput.txt > output.txt\n");
                exit(1);
        }
        assert(argc == 2);

        struct segments *seg = malloc(sizeof(struct segments)+ 268435456 * sizeof(uint32_t *));
        seg->curr_size = 0;
        
        
        FILE *file = fopen(argv[1], "r");
        assert(file != NULL);

        struct stat st;
        stat(argv[1], &st);
        int size = st.st_size;

        uint32_t *instruction;
        instruction = calloc(size, sizeof(uint32_t));
        assert(instruction != NULL);

        struct unmapped *unmapped_ids = malloc(sizeof(struct unmapped) + (268435456 * sizeof(uint32_t)));
        assert(unmapped_ids != NULL);
        unmapped_ids->curr_size = 0;

        struct size_struct *sizes = malloc(sizeof(struct size_struct) + (268435456 * sizeof(int)));
        assert(sizes != NULL);
        int new_size = size/4;
        sizes->sizes[0] = new_size;
        sizes->curr_size = 1;

        uint32_t word;
        for (int j = 0; j < new_size; j++) {
                word = 0;
                for (int i = 24; i >= 0; i = i - 8) {
                        word = ((word >> (8 + i)) << (8 + i)) | ((word << (32 - i)) >> (32 - i)) | (fgetc(file) << i);   
                }
                instruction[j] = word;
        }

        int check_close = fclose(file); 
        assert(check_close == 0);
        
        int arr_size = seg->curr_size;
        seg->arr[arr_size] = instruction;
        seg->curr_size++;

        uint32_t program_counter = 0;
        uint32_t *new_instruction;
        new_instruction = seg->arr[0];

        uint32_t *one, two;
        int temp;
        uint32_t curr_instr = new_instruction[program_counter];
       
        while(1) {      
                
                switch (curr_instr >> 28) {
                        case CMOV:
                                if (registers[curr_instr << 29 >> 29] != 0) {
                                        registers[curr_instr << 23 >> 29] = registers[curr_instr << 26 >> 29];
                                }
                                program_counter++;
                                break;
                        case SLOAD:
                                registers[curr_instr << 23 >> 29] = seg->arr[registers[curr_instr << 26 >> 29]][registers[curr_instr << 29 >> 29]];
                                program_counter++;
                                break;
                        case SSTORE:
                                seg->arr[registers[curr_instr << 23 >> 29]][registers[curr_instr << 26 >> 29]] = registers[curr_instr << 29 >> 29];
                                program_counter++;
                                break;
                        case ADD:
                                registers[curr_instr << 23 >> 29] = registers[curr_instr << 26 >> 29] + registers[curr_instr << 29 >> 29];
                                program_counter++;
                                break;
                        case MUL:
                                registers[curr_instr << 23 >> 29] = (registers[curr_instr << 26 >> 29]) * (registers[curr_instr << 29 >> 29]);
                                program_counter++;
                                break;
                        case DIV:
                                registers[curr_instr << 23 >> 29] = (registers[curr_instr << 26 >> 29]) / (registers[curr_instr << 29 >> 29]);
                                program_counter++;
                                break;
                        case NAND:
                                registers[curr_instr << 23 >> 29] = ~(registers[curr_instr << 26 >> 29] & registers[curr_instr << 29 >> 29]);
                                program_counter++;
                                break;
                        case HALT:
                                temp = seg->curr_size - 1;
                                for (int i = 0; i < temp; i++) {
                                        if(seg->arr[i] != NULL) {
                                                free(seg->arr[i]);      
                                        }
                                        
                                }
                                free(seg);
                                free(sizes);
                                free(unmapped_ids);

                                exit(EXIT_SUCCESS);
                                break;
                        case MAP:
                                if (unmapped_ids->curr_size > 0) {
                                        unmapped_ids->curr_size--;
                                        two = unmapped_ids->id[unmapped_ids->curr_size];
                                        
                                        one = calloc(registers[curr_instr << 29 >> 29], sizeof(uint32_t));
                                        assert(one != NULL);

                                        seg->arr[two] = one;
                                        
                                        sizes->sizes[two] = registers[curr_instr << 29 >> 29];
                                        sizes->curr_size++;
                                } else {
                                        id = id + 1;
                                        two = id;
                                        one = calloc(registers[curr_instr << 29 >> 29], sizeof(uint32_t));
                                        assert(one != NULL);
                                        seg->arr[seg->curr_size] = one;

                                        seg->curr_size++; 
                                        sizes->sizes[sizes->curr_size] = registers[curr_instr << 29 >> 29];
                                        sizes->curr_size++;
                                }
                                
                                registers[curr_instr << 26 >> 29] = two;
                                program_counter++;
                                break;
                        case UNMAP:
                                two = registers[curr_instr << 29 >> 29];
                                one = seg->arr[two];
                                free(one);
                                
                                seg->arr[two] = NULL;

                                unmapped_ids->id[unmapped_ids->curr_size] = two;
                                unmapped_ids->curr_size++;
                                program_counter++;
                                break;
                        case OUT:
                                putchar(registers[curr_instr << 29 >> 29]);
                                program_counter++;
                                break;
                        case IN:
                                if (temp == EOF) {
                                        registers[curr_instr << 29 >> 29] = ~0;
                                } else {
                                        
                                        registers[curr_instr << 29 >> 29] = getchar();
                                }
                                program_counter++;
                                break;
                        case LOADP:
                                two = registers[curr_instr << 26 >> 29];
                                if (two != 0) {
                                        temp = sizes->sizes[two];
                        
                                        uint32_t *duplicate;
                                        duplicate = calloc(temp, sizeof(uint32_t));
                                        assert(duplicate != NULL);

                                        for (int i = 0; i < temp; i++) {
                                                duplicate[i] = seg->arr[two][i];
                                        }
                                        free(seg->arr[0]);
                                        seg->arr[0] = duplicate;
                                        sizes->sizes[0] = temp;
                                }
                                program_counter = registers[curr_instr << 29 >> 29];
                                new_instruction = seg->arr[0];

                                break;
                        case LV:
                                registers[curr_instr << 4 >> 29] = curr_instr << 7 >> 7;
                                program_counter++;
                                break;

                }
                
                curr_instr = new_instruction[program_counter];
        }
        
        return 0;
}