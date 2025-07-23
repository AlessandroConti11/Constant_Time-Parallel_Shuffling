#include <stdio.h>
#include <getopt.h>

#include "insertion_series/insertionSeries.h"
#include "constant-weight_words/constantWeightWord.h"


/**
 * Function that reads the initial list.
 *
 * @param intList the intList to fill.
 */
void read_intline(IntList *intList){
    /// The buffer useful to read the input list.
    char buffer[4096];

    if(!fgets(buffer, sizeof buffer, stdin)) {
        return;
    }

    /// The list of all input inserted by the user.
    char *inputList = strtok(buffer, " \t\r\n");

    while(inputList){
        intlist_append(intList, atoi(inputList));
        inputList = strtok(NULL, " \t\r\n");
    }
}

/**
 * Function that reads the pair position, value to insert.
 *
 * @param pairList the pairList to fill.
 */
void read_pairs(PairList *pairList){
    printf("Insert the pair <position, value>, one per row;\n"
           "Click enter on an empty row to stop the procedure.\n");

    /// The buffer useful to read the pair
    char buffer[256];

    while(1){
        fputs("> ",stdout);
        fflush(stdout);

        if(!fgets(buffer, sizeof buffer, stdin))
            break;
        if(buffer[0] == '\n' || buffer[0] == '\r')
            break;

        /// The first index of the pair.
        int index0;
        /// The second index of the pair.
        int index1;

        if(sscanf(buffer, "%d %d", &index0, &index1) == 2) {
            pairlist_append(pairList, index0, index1);
        }
        else {
            printf("Not a valid format");
        }
    }
}


/**
 * Function that print the intList.
 *
 * @param intList the intList to print.
 */
void print_intlist(const IntList *intList){
    putchar('[');

    for(size_t i = 0; i < intList->listSize; ++i){
        if(i) {
            printf(", ");
        }

        printf("%d", intList->list[i]);
    }

    puts("]");
}


/**
 * Function that displays a help message that describes how to use the program, including the available short and long command-line options.
 *
 * @param progName the program name.
 */
void print_help(const char *progName) {
    printf("Usage: %s [options]\n", progName);
    printf("Options:\n");
    printf("  -p, --parallel     Run in parallel mode\n");
    printf("  -s, --serial       Run in serial mode (default)\n");
    printf("  -h, --help         Show this help message\n");
}


int main(int argc, char **argv) {
    /// Selects the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
    short serialOrParallel = 0;
    /// Defines the possible long options.
    static struct option longOptions[] = {
        {"parallel", no_argument,       0, 'p'},
        {"serial",   no_argument,       0, 's'},
        {"help",     no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    /// Gets the return value of the getopt function, i.e. the chosen option.
    int opt;
    /// Index in the longOptions array indicating which long option has been selected.
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "psh", longOptions, &option_index)) != -1) {
        switch (opt) {
            case 'p':
                serialOrParallel = 1;
            break;
            case 's':
                serialOrParallel = 0;
            break;
            case 'h':
                print_help(argv[0]);
            return 0;
            default:
                print_help(argv[0]);
            return 1;
        }
    }

    printf("Insertion Series of DJB - %s\n\n", serialOrParallel ? "parallel version" : "serial version");

    /// The intList to fill.
    IntList intList;
    intlist_init(&intList);

    printf("Insert the list (number space separated):\n");
    fflush(stdout);
    read_intline(&intList);

    printf("The list before the insertion:\n");
    print_intlist(&intList);

    /// The pairList - position, value.
    PairList pairList;
    pairlist_init(&pairList);

    read_pairs(&pairList);

    IntList list = cww_via_insertionseries(4, &pairList, 0);
    printf("cww: ");
    print_intlist(&list);

    /// The result.
    IntList result = insertionseries(&intList, &pairList, serialOrParallel);

    printf("\nThe list after all the insertion\n");
    print_intlist(&result);

    /// The pairList sorted.
    PairList pairListSort = insertionseries_sort_recursive(&pairList, serialOrParallel);

    puts("\nFinal positions:");
    for(size_t i=0; i < pairListSort.listSize; ++i) {
        printf("position %d: %d\n", pairListSort.list[i].index0, pairListSort.list[i].index1);
    }

    intlist_free(&result);
    intlist_free(&intList);
    pairlist_free(&pairList);
    pairlist_free(&pairListSort);

    return 0;
}
