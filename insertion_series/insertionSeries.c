#include "insertionSeries.h"


/**
 * Function that computes the cumulative prefixes of an intList.
 *
 * @note The first element is always 0.
 *
 * @param list the intList.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the new list that has size input list + 1, where all element is the sum of all previous elements of the input list.
 */
IntList prefixSum(const IntList *list, short parallel) {
    if (parallel) {
        return prefixSumParallel(list);
    }
    else {
        return prefixSumSerial(list);
    }
}

/**
 * Function that computes the cumulative prefixes of an intList.
 *
 * @details Serial version.
 * @note The first element is always 0.
 *
 * @param list the intList.
 * @return the new list that has size input list + 1, where all element is the sum of all previous elements of the input list.
 */
IntList prefixSumSerial(const IntList *list) {
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
 * Function that computes the cumulative prefixes of an intList.
 *
 * @details Parallel version.
 * @note The first element is always 0.
 *
 * @param list the intList.
 * @return the new list that has size input list + 1, where all element is the sum of all previous elements of the input list.
 */
IntList prefixSumParallel(const IntList *list) {
    /// The result.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, list->listSize + 1);

    /// List size.
    size_t listSize = list->listSize;
    /// Output list.
    int *output = malloc((listSize + 1) * sizeof(int));
    output[0] = 0;

    /// Number of threads used.
    int numberThreadUsed = 0;
    /// List of partial sum.
    int *partialSumList = NULL;

#pragma omp parallel
    {
        /// Thread ID.
        int threadID = omp_get_thread_num();
        /// Number of thread.
        int numberThread = omp_get_num_threads();

#pragma omp single
        {
            numberThreadUsed = numberThread;
            partialSumList = calloc(numberThreadUsed, sizeof(int));
        }

        /// Chunk per thread.
        size_t chunk = (listSize + numberThread - 1) / numberThread;
        /// Start position.
        size_t start = threadID * chunk;

        /// The mux selector.
        /// @details start + chunk < listSize --> -1 = 0xFF...FF
        /// @details start + chunk > listSize --> 0  = 0x00...00
        size_t muxSelector = (size_t)(-(start + chunk < listSize));

        /// End position.
        size_t end = ((start + chunk) & muxSelector) | (listSize & !muxSelector);

        /// Local sum value.
        int localSum = 0;

        for (size_t i = start; i < end; ++i) {
            localSum += list->list[i];
            output[i + 1] = localSum;
        }
        partialSumList[threadID] = localSum;

#pragma omp barrier

        /// Offset used to insert thread results into the output list.
        int offset = 0;

        for (int i = 0; i < threadID; ++i) {
            offset += partialSumList[i];
        }

        for (size_t i = start + 1; i <= end; ++i) {
            output[i] += offset;
        }
    }

    for (size_t i = 0; i <= listSize; ++i) {
        intlist_append(&result, output[i]);
    }

    free(output);
    free(partialSumList);

    return result;
}


/**
 * Function that merge and sort two list of quadruple.
 *
 * @param firstList the first list of quadruple.
 * @param firstListSize the size of the first list of quadruple.
 * @param secondList the second list of quadruple.
 * @param secondListSize the size of the second list of quadruple.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the ordered union of the two input lists.
 */
Quadruple *merge(Quadruple *firstList, size_t firstListSize, Quadruple *secondList, size_t secondListSize, short parallel) {
    if (parallel) {
        return mergeParallel(firstList, firstListSize, secondList, secondListSize);
    }
    else {
        return mergeSerial(firstList, firstListSize, secondList, secondListSize);
    }
}

/**
 * Function that merge and sort two list of quadruple.
 *
 * @details Serial version.
 *
 * @param firstList the first list of quadruple.
 * @param firstListSize the size of the first list of quadruple.
 * @param secondList the second list of quadruple.
 * @param secondListSize the size of the second list of quadruple.
 * @return the ordered union of the two input lists.
 */
Quadruple *mergeSerial(Quadruple *firstList, size_t firstListSize, Quadruple *secondList, size_t secondListSize) {
    /// The size of the new list of quadruple.
    size_t resultSize = firstListSize + secondListSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *result = malloc(resultSize * sizeof * result);

    // merge(L, R) = copy the first; copy the second; sort
    memcpy(result, firstList, firstListSize * sizeof * result);
    memcpy(result + firstListSize, secondList, secondListSize * sizeof * result);
    bitonicSort(result, 0, resultSize, ASCENDING, SERIAL);

    return result;
}

/**
 * Merge two sorted arrays of Quadruple into a single sorted array.
 *
 * @details Parallel version.
 *
 * @param firstList the first list of quadruple.
 * @param firstListSize the size of the first list of quadruple.
 * @param secondList the second list of quadruple.
 * @param secondListSize the size of the second list of quadruple.
 * @return the ordered union of the two input lists.
 */
Quadruple *mergeParallel(Quadruple *firstList, size_t firstListSize, Quadruple *secondList, size_t secondListSize) {
    // The size of the new list of quadruple.
    size_t resultSize = firstListSize + secondListSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *result = malloc(resultSize * sizeof * result);

    // merge(L, R) = copy the first; copy the second; sort
#pragma omp parallel sections
    {
#pragma omp section
        memcpy(result, firstList, firstListSize * sizeof *result);

#pragma omp section
        memcpy(result + firstListSize, secondList, secondListSize * sizeof *result);
    }

    bitonicSort(result, 0, resultSize, ASCENDING, PARALLEL);

    return result;
}


/**
 * Function that performs an ordered merging of two pairLists.
 *
 * @param firstList the first pairList.
 * @param secondList the second pairList.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the ordered merged pairList.
 */
PairList insertionseries_sort_merge(const PairList *firstList, const PairList *secondList, short parallel) {
    /// List size of firstList.
    size_t firstListSize = firstList->listSize;
    /// List size of secondList.
    size_t secondListSize = secondList->listSize;

    /// Array of quadruple, each quadruple value corresponds to the firstList values.
    Quadruple *firstListQuadrupleArray = malloc(firstListSize * sizeof * firstListQuadrupleArray);
    /// Array of quadruple, each quadruple value corresponds to the secondList values.
    Quadruple *secondListQuadrupleArray = malloc(secondListSize * sizeof * secondListQuadrupleArray);

    // create quadruple arrays - [<key, value, fromLeft, indexInOriginalList>]
    if (parallel) {
#pragma omp parallel
        {
#pragma omp for schedule(static) nowait
            for(size_t j = 0; j < firstListSize; ++j) {
                firstListQuadrupleArray[j].index0 = firstList->list[j].index0;
                firstListQuadrupleArray[j].index1 = firstList->list[j].index1;
                firstListQuadrupleArray[j].fromLeft = 1;
                firstListQuadrupleArray[j].indexInItsList = 0;
            }

#pragma omp for schedule(static)
            // normalize the index 0
            for(size_t j = 0; j < secondListSize; ++j) {
                secondListQuadrupleArray[j].index0 = secondList->list[j].index0 - (int)j;
                secondListQuadrupleArray[j].index1 = secondList->list[j].index1;
                secondListQuadrupleArray[j].fromLeft = 0;
                secondListQuadrupleArray[j].indexInItsList = (int)j;
            }
        }
    }
    else {
        for(size_t j = 0; j < firstListSize; ++j) {
            firstListQuadrupleArray[j].index0 = firstList->list[j].index0;
            firstListQuadrupleArray[j].index1 = firstList->list[j].index1;
            firstListQuadrupleArray[j].fromLeft = 1;
            firstListQuadrupleArray[j].indexInItsList = 0;
        }

        // normalize the index 0
        for(size_t j = 0; j < secondListSize; ++j) {
            secondListQuadrupleArray[j].index0 = secondList->list[j].index0 - (int)j;
            secondListQuadrupleArray[j].index1 = secondList->list[j].index1;
            secondListQuadrupleArray[j].fromLeft = 0;
            secondListQuadrupleArray[j].indexInItsList = (int)j;
        }
    }

    /// The size of the new list of quadruple.
    size_t newQuadrupleArraySize = firstListSize + secondListSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *newQuadrupleArray = merge(firstListQuadrupleArray, firstListSize, secondListQuadrupleArray, secondListSize, parallel);


    /// IntList that contains the inverse of newQuadrupleArray.fromLeft.
    IntList fromLeftInverse;
    intlist_init(&fromLeftInverse);
    intlist_reserve(&fromLeftInverse, newQuadrupleArraySize);

    for(size_t i = 0; i < newQuadrupleArraySize; ++i) {
        intlist_append(&fromLeftInverse, 1 - newQuadrupleArray[i].fromLeft);
    }

    /// The list of true offset to add at each element of newQuadrupleArray.index0.
    IntList offsetList = prefixSum(&fromLeftInverse, parallel);

    /// The output pairList.
    PairList result;
    pairlist_init(&result);
    pairlist_reserve(&result, newQuadrupleArraySize);

    for(size_t i = 0; i < newQuadrupleArraySize; ++i) {
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
 * @note The function works recursively.
 *
 * @param pairList the pairList to sort.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the pairList sorted.
 */
PairList insertionseries_sort_recursive(const PairList *pairList, short parallel) {
    // base case
    if (pairList->listSize <= 1) {
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

    for(size_t i = 0; i < halfPairListSize; ++i) {
        pairlist_append(&left, pairList->list[i].index0, pairList->list[i].index1);
    }
    for(size_t i = halfPairListSize; i < pairListSize; ++i) {
        pairlist_append(&right, pairList->list[i].index0, pairList->list[i].index1);
    }

    /// The sorted left part of the input pairList.
    PairList sortedLeft = insertionseries_sort_recursive(&left, parallel);
    /// The sorted right part of the input pairList
    PairList sortedRight = insertionseries_sort_recursive(&right, parallel);
    /// The initial sorted pairList.
    PairList result = insertionseries_sort_merge(&sortedLeft, &sortedRight, parallel);

    pairlist_free(&left);
    pairlist_free(&right);
    pairlist_free(&sortedLeft);
    pairlist_free(&sortedRight);

    return result;
}

/**
 * Function that inserts a list of values at specific positions in a list.
 *
 * @param list the intList where to insert the new values.
 * @param pairList the pairList that contains the positions and the values to insert in the intList.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the new intList with the value inserted.
 */
IntList insertionseries_merge_after_sort_recursive(const IntList *list, const PairList *pairList, short parallel) {
    /// The pairList that contains all the value in the list - <actual_position, element>.
    PairList listPair;
    pairlist_init(&listPair);

    for(size_t i = 0; i < list->listSize; ++i) {
        pairlist_append(&listPair, (int) i, list->list[i]);
    }

    /// The pairList of positions where to insert an ordered element.
    /// @details The index is modified. Now it is the actual index where the element must be inserted.
    /// @note The pairs are sorted by index.
    PairList pairListSorted = insertionseries_sort_recursive(pairList, parallel);
    /// The new sorted pairList containing the original list and the inserted elements.
    PairList finalPairList = insertionseries_sort_merge(&listPair, &pairListSorted, parallel);

    /// The new intList with the value inserted.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, finalPairList.listSize);

    for(size_t i = 0; i < finalPairList.listSize; ++i) {
        intlist_append(&result, finalPairList.list[i].index1);
    }

    pairlist_free(&listPair);
    pairlist_free(&pairListSorted);
    pairlist_free(&finalPairList);

    return result;
}
