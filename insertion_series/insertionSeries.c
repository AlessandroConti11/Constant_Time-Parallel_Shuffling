#include "insertionSeries.h"


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
 * Function that search the position of a key.
 *
 * @param list the list where to look for the key.
 * @param start the position from which to start searching for the key.
 * @param end the position to search for the key.
 * @param key the key to search.
 * @return the position of the key.
 */
size_t binarySearch(Quadruple *list, size_t start, size_t end, Quadruple key) {
    /// The low index.
    size_t low = start;
    /// The high index.
    size_t high = end;

    while (low < high) {
        /// The mid position.
        size_t mid = low + (high - low) / 2;

        if (quadrupleComparison(&list[mid], &key) < 0) {
            low = mid + 1;
        }
        else {
            high = mid;
        }
    }

    return low;
}


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
        /// End position.
        size_t end = (start + chunk < listSize) ? start + chunk : listSize;

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
    qsort(result, resultSize, sizeof * result, quadrupleComparison);

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
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *result = malloc(firstListSize + secondListSize * sizeof(Quadruple));
    if (!result) {
        return NULL;
    }

#pragma omp parallel
    {
#pragma omp for schedule(static) nowait
        for (size_t i = 0; i < firstListSize; i++) {
            result[i + binarySearch(secondList, 0, secondListSize, firstList[i])] = firstList[i];
        }

#pragma omp for schedule(static)
        for (size_t i = 0; i < secondListSize; i++) {
            result[i + binarySearch(firstList, 0, firstListSize, secondList[i])] = secondList[i];
        }
    }

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

    if (parallel) {
#pragma omp parallel
        {
#pragma omp for schedule(static) nowait
            for(size_t j = 0; j < firstListSize; ++j){
                firstListQuadrupleArray[j].index0 = firstList->list[j].index0;
                firstListQuadrupleArray[j].fromLeft = 1;
                firstListQuadrupleArray[j].indexInItsList = 0;
                firstListQuadrupleArray[j].index1 = firstList->list[j].index1;
            }

#pragma omp for schedule(static)
            // to the second list we want to give it more importance (they are the tuples not yet entered)
            for(size_t j = 0; j < secondListSize; ++j){
                secondListQuadrupleArray[j].index0 = secondList->list[j].index0 - (int)j;
                secondListQuadrupleArray[j].fromLeft = 0;
                secondListQuadrupleArray[j].indexInItsList = (int)j;
                secondListQuadrupleArray[j].index1 = secondList->list[j].index1;
            }
        }
    }
    else {
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
    }

    /// The size of the new list of quadruple.
    size_t newQuadrupleArraySize = firstListSize + secondListSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *newQuadrupleArray = merge(firstListQuadrupleArray, firstListSize, secondListQuadrupleArray, secondListSize, parallel);

    // let us compute the correct offsetList to add to newQuadrupleArray.index0
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
 * @note The function work recursively.
 *
 * @param pairList the pairList to sort.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the pairList sorted.
 */
PairList insertionseries_sort_recursive(const PairList *pairList, short parallel) {
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

    /// The sorted pairList.
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
    /// The pairList that contains all the value in the list, <actual_position, element>.
    PairList listPair;
    pairlist_init(&listPair);

    for(size_t i = 0; i < list->listSize; ++i) {
        pairlist_append(&listPair, (int) i, list->list[i]);
    }

    /// The pairList sorted.
    PairList pairListSorted = insertionseries_sort_recursive(pairList, parallel);
    /// The new sorted pairList that contains listPair and pairList.
    PairList fusionSort = insertionseries_sort_merge(&listPair, &pairListSorted, parallel);

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
