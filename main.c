#include <stdio.h>
#include <string.h>
#include <assert.h>
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
    printf("      --insertionseries   Execute the insertionSeries function\n");
    printf("      --cww               Execute the constant-weight word creation function\n");
    printf("  -p, --parallel          Run in parallel mode\n");
    printf("  -s, --serial            Run in serial mode (default)\n");
    printf("  -h, --help              Show this help message\n");
}


/**
 * Function that shows the Daniel J. Bernstein's insertion series algorithm.
 *
 * @param serialOrParallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 */
void mainInsertionSeries(short serialOrParallel) {
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

    /// The list with the required elements inserted in the required positions.
    IntList listWithNewElement = insertionseries(&intList, &pairList, serialOrParallel);

    printf("\nThe list after all the insertion\n");
    print_intlist(&listWithNewElement);

    intlist_free(&listWithNewElement);
    intlist_free(&intList);
    pairlist_free(&pairList);
}

/**
 * Function that shows the Daniel J. Bernstein's constant-weight word creation algorithm.
 *
 * @param serialOrParallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 */
void mainCww(short serialOrParallel) {
    printf("Constant-Weight Word Construction of DJB - %s\n\n", serialOrParallel ? "parallel version" : "serial version");

    /// The number of 0s in the constant-weight word.
    int numberOfZero = 0;

    printf("Insert the number of 0s: ");
    int returnScanf = scanf("%d", &numberOfZero);
    assert(returnScanf > 0);

    /// Integer used only to read the \n or EOF after the scanf
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    /// The intList of positions where the 1s will go.
    IntList positionOfOne;
    intlist_init(&positionOfOne);

    printf("Insert the list of positions where to insert the 1s (number space separated):\n");
    fflush(stdout);
    read_intline(&positionOfOne);

    /// The constant-weight word created using the cww function.
    IntList result_cww = cww(numberOfZero, &positionOfOne, serialOrParallel);
    /// The constant-weight word created using the cww_via_insertionseries function.
    IntList result_cww_via_insertionseries = cww_via_insertionseries(numberOfZero, &positionOfOne, serialOrParallel);

    printf("Creation of cww using the cww function:\n");
    print_intlist(&result_cww);
    printf("\nCreation of cww using the cww_via_insertionseries function:\n");
    print_intlist(&result_cww_via_insertionseries);

    intlist_free(&positionOfOne);
    intlist_free(&result_cww);
    intlist_free(&result_cww_via_insertionseries);
}

int main(int argc, char **argv) {
    /// Selects the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
    short serialOrParallel = 0;
    /// Selects the algorithm to be execute, either cww, 1, or insertionSeries, 0.
    short algorithm = 0;

    /// Defines the possible long options.
    static struct option longOptions[] = {
        {"insertionseries", no_argument, 0, 0},
        {"cww", no_argument, 0, 0},
        {"parallel", no_argument, 0, 'p'},
        {"serial", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
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
            case 0: // only long options
                if (!strncmp(longOptions[option_index].name, "insertionseries", strlen("insertionseries"))) {
                    algorithm = 0;
                }
                else if (!strncmp(longOptions[option_index].name, "cww", strlen("cww"))) {
                    algorithm = 1;
                }
                break;
            default:
                print_help(argv[0]);
                return 1;
        }
    }

    if (algorithm) {
        mainCww(serialOrParallel);
    }
    else {
        mainInsertionSeries(serialOrParallel);
    }

    return 0;
}
