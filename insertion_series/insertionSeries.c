#include "insertionSeries.h"

/**
 * Function that computes the cumulative prefixes of an intList.
 *
 * @note The first element is always 0.
 *
 * @param list the intList.
 * @return the new list that has size input list + 1, where all element is the sum of all previous elements of the input list.
 */
IntList prefixSum(const IntList *list) {
    /// The result.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, list->listSize + 1);

    /// The partial sum of each element.
    int sum = 0;

    intlist_append(&result, 0);
    for(size_t i=0; i < list->listSize; ++i) {
        sum += list->list[i];
        intlist_append(&result, sum);
    }

    return result;
}

/**
 * Function that compares two quadruple.
 *
 * @note The comparison compare the following quadruple's value:
 * 1. index0;
 * 2. fromLeft;
 * 3. indexInItsList.
 *
 * @param firstQuadruple the first quadruple to compare.
 * @param secondQuadruple the second quadruple to compare.
 * @return firstQuadruple negative value if the first quadruple is less than the first. 0 if the two quadruples are equal. A positive value if the first quadruple is greater.
 */
int quadrupleComparison(const void *firstQuadruple, const void *secondQuadruple) {
    /// First quadruple value.
    const Quadruple *first = firstQuadruple;
    /// Second quadruple value.
    const Quadruple *second = secondQuadruple;

    if(first->index0 != second->index0) {
        return (first->index0 > second->index0) - (first->index0 < second->index0);
    }

    if(first->fromLeft != second->fromLeft) {
        return (first->fromLeft > second->fromLeft) - (first->fromLeft < second->fromLeft);
    }

    return (first->indexInItsList > second->indexInItsList) - (first->indexInItsList < second->indexInItsList);
}

/**
 * Function that performs an ordered merging of two pairLists.
 *
 * @param firstList the first pairList.
 * @param secondList the second pairList.
 * @return the ordered merged pairList.
 */
PairList insertionseries_sort_merge(const PairList *firstList, const PairList *secondList) {
    /// List size of firstList.
    size_t firstListSize = firstList->listSize;
    /// List size of secondList.
    size_t secondListSize = secondList->listSize;

    /// Array of quadruple, each quadruple value corresponds to the firstList values.
    Quadruple *firstListQuadrupleArray = malloc(firstListSize * sizeof * firstListQuadrupleArray);
    /// Array of quadruple, each quadruple value corresponds to the secondList values.
    Quadruple *secondListQuadrupleArray = malloc(secondListSize * sizeof * secondListQuadrupleArray);

    for(size_t j = 0; j < firstListSize; ++j){
        firstListQuadrupleArray[j].index0 = firstList->list[j].index0;
        firstListQuadrupleArray[j].fromLeft = 1;
        firstListQuadrupleArray[j].indexInItsList = 0;
        firstListQuadrupleArray[j].index1 = firstList->list[j].index1;
    }
    // to the second list we want to give it more importance (they are the tuples not yet entered)
    for(size_t j = 0; j < secondListSize; ++j){
        secondListQuadrupleArray[j].index0 = secondList->list[j].index0 - (int)j;
        secondListQuadrupleArray[j].fromLeft = 0;
        secondListQuadrupleArray[j].indexInItsList = (int)j;
        secondListQuadrupleArray[j].index1 = secondList->list[j].index1;
    }

    /// The size of the new list of quadruple.
    size_t newQuadrupleArraySize = firstListSize + secondListSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *newQuadrupleArray = malloc(newQuadrupleArraySize * sizeof * newQuadrupleArray);

    // merge(L, R) = copy the first; copy the second; sort
    memcpy(newQuadrupleArray, firstListQuadrupleArray, firstListSize * sizeof*newQuadrupleArray);
    memcpy(newQuadrupleArray + firstListSize, secondListQuadrupleArray, secondListSize * sizeof*newQuadrupleArray);
    qsort(newQuadrupleArray, newQuadrupleArraySize, sizeof * newQuadrupleArray, quadrupleComparison);

    // let us compute the correct offsetList to add to newQuadrupleArray.index0
    /// IntList that contains the inverse of newQuadrupleArray.fromLeft.
    IntList fromLeftInverse;
    intlist_init(&fromLeftInverse);
    intlist_reserve(&fromLeftInverse, newQuadrupleArraySize);

    for(size_t i=0; i < newQuadrupleArraySize; ++i) {
        intlist_append(&fromLeftInverse, 1 - newQuadrupleArray[i].fromLeft);
    }

    /// The list of true offset to add at each element of newQuadrupleArray.index0.
    IntList offsetList = prefixSum(&fromLeftInverse);

    /* Step‑4: produce final PairList */
    /// The output pairList.
    PairList result;
    pairlist_init(&result);
    pairlist_reserve(&result, newQuadrupleArraySize);

    for(size_t i=0; i < newQuadrupleArraySize; ++i) {
        size_t position = newQuadrupleArray[i].index0 + offsetList.list[i];

        if(position <= 0 || position > newQuadrupleArraySize) {
            fprintf(stderr, "Error: computed insertion index %zu out of bounds [0, %zu]\n", position, newQuadrupleArraySize);
            exit(EXIT_FAILURE);
        }
        pairlist_append(&result, newQuadrupleArray[i].index0 + offsetList.list[i], newQuadrupleArray[i].index1);
    }

    // clean the allocated list
    free(firstListQuadrupleArray);
    free(secondListQuadrupleArray);
    free(newQuadrupleArray);
    intlist_free(&fromLeftInverse);
    intlist_free(&offsetList);

    return result;
}

/**
 * Function that sorts a pairList.
 *
 * @note The function work recursively.
 *
 * @param pairList the pairList to sort.
 * @return the pairList sorted.
 */
PairList insertionseries_sort_recursive(const PairList *pairList) {
    // base case
    if(pairList->listSize <= 1){
        /// The sorted pairList.
        PairList result;

        pairlist_copy(&result, pairList);

        return result;
    }

    /// The pairList size.
    size_t pairListSize = pairList->listSize;
    /// The half of pairListSize.
    size_t halfPairListSize = pairListSize / 2;

    /// The left part of the input pairList.
    PairList left;
    pairlist_init(&left);
    pairlist_reserve(&left, halfPairListSize);
    /// The right part of the input pairList.
    PairList right;
    pairlist_init(&right);
    pairlist_reserve(&right, pairListSize - halfPairListSize);

    for(size_t i=0; i < halfPairListSize; ++i) {
        pairlist_append(&left, pairList->list[i].index0, pairList->list[i].index1);
    }
    for(size_t i=halfPairListSize; i < pairListSize; ++i) {
        pairlist_append(&right, pairList->list[i].index0, pairList->list[i].index1);
    }

    /// The left pairList sorted.
    PairList leftSort = insertionseries_sort_recursive(&left);
    /// The right pairList sorted.
    PairList rightSort = insertionseries_sort_recursive(&right);
    /// The sorted pairList.
    PairList result = insertionseries_sort_merge(&leftSort, &rightSort);

    pairlist_free(&left);
    pairlist_free(&right);
    pairlist_free(&leftSort);
    pairlist_free(&rightSort);

    return result;
}

/**
 * Function that inserts a list of values at specific positions in a list.
 *
 * @param list the intList where to insert the new values.
 * @param pairList the pairList that contains the positions and the values to insert in the intList.
 * @return the new intList with the value inserted.
 */
IntList insertionseries_merge_after_sort_recursive(const IntList *list, const PairList *pairList) {
    /// The pairList that contains all the value in the list, <actual_position, element>.
    PairList listPair;
    pairlist_init(&listPair);

    for(size_t i=0; i < list->listSize; ++i) {
        pairlist_append(&listPair, (int) i, list->list[i]);
    }

    /// The pairList sorted.
    PairList pairListSorted = insertionseries_sort_recursive(pairList);
    /// The new sorted pairList that contains listPair and pairList.
    PairList fusionSort = insertionseries_sort_merge(&listPair, &pairListSorted);

    /// The new intList with the value inserted.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, fusionSort.listSize);

    for(size_t i = 0; i < fusionSort.listSize; ++i) {
        intlist_append(&result, fusionSort.list[i].index1);
    }

    pairlist_free(&listPair);
    pairlist_free(&pairListSorted);
    pairlist_free(&fusionSort);

    return result;
}


#define insertionseries insertionseries_merge_after_sort_recursive