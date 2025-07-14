#include "insertion_series/insertionSeries.h"


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


//int argc,char **argv
int main() {
    printf("Insertion Series of DJB\n\n");

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

    /// The result.
    IntList result = insertionseries(&intList, &pairList);

    printf("\nThe list after all the insertion\n");
    print_intlist(&result);

    /// The pairList sorted.
    PairList pairListSort = insertionseries_sort_recursive(&pairList);

    puts("\nFinal positions:");
    for(size_t i=0; i < pairListSort.listSize; ++i) {
        printf("position %d: %d\n", pairListSort.list[i].index0, pairListSort.list[i].index1);
    }

    intlist_free(&result);
    pairlist_free(&pairListSort);

    intlist_free(&intList);
    pairlist_free(&pairList);

    return 0;
}
