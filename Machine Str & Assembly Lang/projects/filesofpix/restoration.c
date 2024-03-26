#include "restoration.h"
#include "string.h"
#include "atom.h"
#include "table.h"
#include "seq.h"


/* main
  *    Purpose: Deals with file I/O and calls restore
  * Parameters: int argc, char *argv[]
  *             argc is the number of arguments in the command line
  *             and argv holds the names of the commands
  *    Returns: EXIT_SUCCESS
  * Effect: N/A
  * Errors: asserts that there is at most one argument and assertion
  *         to ensure file is closed
  */
int main(int argc, char *argv[]) {

        assert(argc <= 2); /* so that at most only one command is taken in */
        if (argc == 1) {
                restore(stdin);
        }
        else if (argc == 2) {
                FILE *file = open_file(argv[1], "r"); /* open given file */
                restore(file);
                
                int x = fclose(file); /* close file */
                assert(x==0);
        }
    
        return 0;
}

/* open_file
  *    Purpose: Opens file of given name in specified mode
  * Parameters: a pointer to fname and mode.
  *             fname is the name of the file and mode is the mode of opening 
  *             (reading, writing etc.)
  *    Returns: Pointer to a filestream object pointing to open file/stdin.
  * Effect: N/A
  * Errors: asserts whether the filestream object is non-null to prevent
  *         filestream errors (file is not open, file does not exist etc.)
  */
static FILE *open_file(char *fname, char *mode) {
        FILE *fp = fopen(fname, mode);
        assert(fp != NULL);
        return fp;
}

/* restore
  *    Purpose: Does all the main operations of the restorations program.
                It loops through the open file, calls readaline to read
                lines from the file, calls atom_populator to separate the
                digit and non-digit character. It initialises an atom, 
                sequence, and tree: it populates the atom with the
                non-digit characters, populates the sequence with the
                digit characters, and populates the table with
                atoms as keys and values as sequences. It appends digit
                characters associated with the same atom to the same 
                sequence in the table.
  * Parameters: a pointer to a filestream object which points to the open
  *             file.
  *    Returns: N/A
  * Effects: Runs and organises the main functions of restore. It calls 
  *          other functions to do the actual processing but holds all
  *          the data structures and data.
  *    Errors: Vulnerable to Hanson table, Hanson Sequence, and Hanson 
  *            atom exceptions
  */
static void restore(FILE *fp) {
    char *reader_arr, *digits, *non_digits;
    Table_T table = Table_new(100, NULL, NULL); /* creates a Hanson table */
    Seq_T og_seq; /* sequence that will hold original rasters */
    const char *og_atom; /* atom corresponding to og_seq */
    int atom_length;

        while (!feof(fp)) {
                size_t num_bytes = readaline(fp, &reader_arr);

                if (num_bytes == 0) continue;
                atom_length = atom_populator(num_bytes, reader_arr, &digits, 
                                                &non_digits);
                const char *atom = Atom_new(non_digits, atom_length);
                free(non_digits);

                Seq_T return_val = Table_get(table, atom);
                if (return_val != NULL){
                        Seq_addhi(return_val, digits);
                        og_atom = atom;
                }
                else {
                        Seq_T values = Seq_new(100);
                        Seq_addhi(values, digits);
                        Table_put(table, atom, values);
                }

                free(reader_arr);
        }
        og_seq = Table_get(table, og_atom);
        print_pic(&og_seq);
        Table_map(table, free_table, NULL);
        Table_free(&table);
}

/* atom_populator
  *    Purpose: It splits the read lines into non-digit characters and
                digit characters. Respectively populates digit_array and
                nondigit_array using pass-by-reference.
  * Parameters: num_bytes is the number of bytes in the line read by
  *             readaline. *chars is an array of characters (interpreted as
  *             a string) - it holds all the characters in the line read
  *             by readaline. **digit_array is a pointer to an array of
  *             digits that exists in restore (it has been passed in by 
  *             reference). **nondigit_array is a pointer to an array of
  *             non-digit characters that also exists in restore (also
  *             passed in by reference). These are populated by reference
  *             with the digit and non-digit characters respectively.
  *    Returns: int nondigit_count - the number of non-digit characters
  *             in the line. This represents the length of the atom to be
  *             declared.
  * Effects: Populates the digit_array passed in by restore with digit
  *          characters, and populates the nondigit_array passed in 
  *          by restore with non-digit characters. Does all this
  *          by reference.
  *    Errors: assertions to ensure there are no memory allocation 
  *           errors
  */
static int atom_populator(int num_bytes, char *chars, char **digit_array, 
                          char **nondigit_array) {
    
        *digit_array = (char *) malloc(num_bytes + 1);
        assert(*digit_array != NULL);
        *nondigit_array = (char *) malloc(num_bytes);
        assert(*nondigit_array != NULL);
        int nondigit_count = 0, digit_count = 0, number = 0;

        for (int i = 0; i < num_bytes; i++) {
                if (chars[i] < 48 || chars[i] > 57) {
                        (*nondigit_array)[nondigit_count] = chars[i];
                        nondigit_count++; /* atom length */
                } 
                else {
                        number = number * 10 + (chars[i] - 48);
                        if (chars[i+1] < 48 || chars[i+1] > 57) {
                                (*digit_array)[digit_count] = number;
                                digit_count++;
                                number = 0;
                        }
                }
        }
        (*digit_array)[digit_count] = '\0';
        return nondigit_count;
}

/* print_pic
  *    Purpose: Prints out a perfectly formatted 'raw' pgm file created after
  *             the uncorrupted process to stdout
  * Parameters: Seq_T *sequence is a pointer to the sequence holding
  *             all the original digit characters. 
  *    Returns: N/A
  * Effects: N/A
  *    Errors: N/A
  */
static void print_pic(Seq_T *sequence)
{
    int height = Seq_length(*sequence); /* height of rasters = length of 
                                                        original sequence */
    int width = strlen(Seq_get(*sequence, 0)); /* width of rasters = length of 
                                 string at any index of original sequence */
    printf("P5\n"); /* magic number of a raw pgm = P5 */
    printf("%d %d\n", width, height);
    printf("255\n"); /* given maxval = 255 */
    for (int i = 0; i < height; i++) {
        char *line = Seq_get(*sequence, i);
        for (int i = 0; i < width; i++)
        {
            printf("%c",line[i]);
        }
    }

}

/* free_table
  *    Purpose: Frees the sequences within the table and the contents
  *             of those sequences too. It has a structure fit to be 
  *             passed into the Table_map() function.
  * Parameters: const void *key is a pointer to an atom which
  *             is a key in the table, void **value is a pointer to a pointer
  *             which points to a value in the table, and void *cl is a 
  *             closure - an application specific pointer
  *    Returns: N/A
  * Effects: Frees all the dynamically allocated memory used by Hanson's
  *          table, sequence, and the contents of the sequence.
  *    Errors: (void) key and (void) cl to prevent 'unused variable' errors
  */
static void free_table(const void *key, void **value, void *cl) {
    Seq_T *temp = (Seq_T *)value;

    for (int i = 0; i < Seq_length(*temp); i++) { /* frees every index of the
                                                    sequence */
        char *digits = Seq_get(*temp, i);
        free(digits);
    }
    Seq_free(temp);

    (void) key;
    (void) cl;
}
