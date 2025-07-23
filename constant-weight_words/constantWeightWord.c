#include "constantWeightWord.h"


/**
 * Function that creates a constant-weight word.
 *
 * @details this function uses only the insertionSeries procedure.
 *
 * @param numberOfZero the number of 0s in the constant-weight word.
 * @param positionOfOne the positions in which to insert the ones.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the constant-weight word.
 */
IntList cww_via_insertionseries(int numberOfZero, IntList *positionOfOne, short parallel) {
    /// The constant-weight word to be created.
    IntList list;
    intlist_init(&list);
    intlist_reserve(&list, numberOfZero);

    for (int i = 0; i < numberOfZero; ++i) {
        intlist_append(&list, 0);
    }

    /// The list of positions in which to enter the value one.
    PairList pairList;
    pairlist_init(&pairList);
    pairlist_reserve(&pairList, positionOfOne->listSize);

    for (size_t i = 0; i < positionOfOne->listSize; ++i) {
        pairlist_append(&pairList, positionOfOne->list[i], 1);
    }

    return insertionseries(&list, &pairList, parallel);
}

/**
 * Function that inserts 1s in the correct position to create a constant-weight word.
 *
 * @param positionOfZero the positions of the 0s within the word.
 * @param positionOfOne the positions of the 1s within the word.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the constant-weight word with the 1s in correct position.
 */
IntList cww_sort_mergebits(const IntList *positionOfZero, const IntList *positionOfOne, short parallel) {
    /// List size of positionOfZero.
    size_t positionOfZeroSize = positionOfZero->listSize;
    /// List size of positionOfOne.
    size_t positionofOneSize = positionOfOne->listSize;

    /// Array of quadruple, each quadruple value corresponds to the firstList values.
    Quadruple *firstListQuadrupleArray = malloc(positionOfZeroSize * sizeof(Quadruple));
    /// Array of quadruple, each quadruple value corresponds to the firstList values.
    Quadruple *secondListQuadrupleArray = malloc(positionofOneSize * sizeof(Quadruple));

    if (parallel) {
#pragma omp parallel
        {
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < positionOfZeroSize; ++i) {
                firstListQuadrupleArray[i].index0 = positionOfZero->list[i];
                firstListQuadrupleArray[i].fromLeft = 1;
                firstListQuadrupleArray[i].indexInItsList = i;
                firstListQuadrupleArray[i].index1 = 1;
            }

#pragma omp for schedule(static) nowait
            // to the second list we want to give it more importance (they are the tuples not yet entered)
            for (size_t j = 0; j < positionofOneSize; ++j) {
                secondListQuadrupleArray[j].index0 = positionOfOne->list[j] - (int)j;
                secondListQuadrupleArray[j].fromLeft = 0;
                secondListQuadrupleArray[j].indexInItsList = j;
                secondListQuadrupleArray[j].index1 = 1;
            }
        }
    }
    else {
        for (size_t i = 0; i < positionOfZeroSize; ++i) {
            firstListQuadrupleArray[i].index0 = positionOfZero->list[i];
            firstListQuadrupleArray[i].fromLeft = 1;
            firstListQuadrupleArray[i].indexInItsList = i;
            firstListQuadrupleArray[i].index1 = 1;
        }

        // to the second list we want to give it more importance (they are the tuples not yet entered)
        for (size_t j = 0; j < positionofOneSize; ++j) {
            secondListQuadrupleArray[j].index0 = positionOfOne->list[j] - (int)j;
            secondListQuadrupleArray[j].fromLeft = 0;
            secondListQuadrupleArray[j].indexInItsList = j;
            secondListQuadrupleArray[j].index1 = 1;
        }
    }

    /// The size of the new list of quadruple.
    size_t newQuadrupleArraySize = positionOfZeroSize + positionofOneSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *newQuadrupleArray = merge(firstListQuadrupleArray, positionOfZeroSize, secondListQuadrupleArray, positionofOneSize, parallel);

    /// The output intList
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, newQuadrupleArraySize);

    for (size_t i = 0; i < newQuadrupleArraySize; ++i) {
        intlist_append(&result, 1 - newQuadrupleArray[i].fromLeft);
    }

    // clean the allocated list
    free(firstListQuadrupleArray);
    free(secondListQuadrupleArray);
    free(newQuadrupleArray);

    return result;
}

/**
 * Function that performs an ordered merging of two intLists.
 *
 * @param firstList the first intList.
 * @param secondList the second intList.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the ordered merged intList.
 */
IntList cww_sort_mergepos(const IntList *firstList, const IntList *secondList, short parallel) {
    /// List size of firstList.
    size_t firstListSize = firstList->listSize;
    /// List size of secondList.
    size_t secondListSize = secondList->listSize;

    /// Array of quadruple, each quadruple value corresponds to the firstList values.
    Quadruple *firstListQuadrupleArray = malloc(firstListSize * sizeof(Quadruple));
    /// Array of quadruple, each quadruple value corresponds to the secondList values.
    Quadruple *secondListQuadrupleArray = malloc(secondListSize * sizeof(Quadruple));

    for (size_t i = 0; i < firstListSize; ++i) {
        firstListQuadrupleArray[i].index0 = firstList->list[i];
        firstListQuadrupleArray[i].fromLeft = 1;
        firstListQuadrupleArray[i].indexInItsList = i;
        firstListQuadrupleArray[i].index1 = 1;
    }

    for (size_t j = 0; j < secondListSize; ++j) {
        secondListQuadrupleArray[j].index0 = secondList->list[j] - (int)j;
        secondListQuadrupleArray[j].fromLeft = 0;
        secondListQuadrupleArray[j].indexInItsList = j;
        secondListQuadrupleArray[j].index1 = 1;
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

    for (size_t i = 0; i < newQuadrupleArraySize; ++i) {
        intlist_append(&fromLeftInverse, 1 - newQuadrupleArray[i].fromLeft);
    }

    /// The list of true offset to add at each element of newQuadrupleArray.index0.
    IntList offsetList = prefixSum(&fromLeftInverse, parallel);

    /// The output intList.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, newQuadrupleArraySize);

    for (size_t i = 0; i < newQuadrupleArraySize; ++i) {
        intlist_append(&result, newQuadrupleArray[i].index0 + offsetList.list[i]);
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
 * Function that sorts an intList.
 *
 * @note The function works recursively.
 *
 * @param intList the intList to sort.
 * @param parallel the type of algortihm execution, either parallel mode, 1, or serial mode, 0.
 * @return the intList sorted.
 */
IntList cww_sort_recursive(const IntList *intList, short parallel) {
    // base case
    if (intList->listSize <= 1) {
        /// The sorted intList.
        IntList result;

        intlist_copy(&result, intList);

        return result;
    }

    /// The intList size.
    size_t intListSize = intList->listSize;
    /// THe half of intListSize.
    size_t halfIntListSize = intListSize / 2;

    /// The left part of the input intList.
    IntList left;
    intlist_init(&left);
    intlist_reserve(&left, halfIntListSize);

    /// The right part of the input intList.
    IntList right;
    intlist_init(&right);
    intlist_reserve(&right, intListSize - halfIntListSize);

    for (size_t i = 0; i < halfIntListSize; ++i) {
        intlist_append(&left, intList->list[i]);
    }

    for (size_t i = halfIntListSize; i < intListSize; ++i) {
        intlist_append(&right, intList->list[i]);
    }


    /// The sorted left part of the input intList.
    IntList sortedLeft = cww_sort_recursive(&left, parallel);
    /// The sorted right part of the input intList.
    IntList sortedRight = cww_sort_recursive(&right, parallel);

    /// The sorted intList.
    IntList result = cww_sort_mergepos(&sortedLeft, &sortedRight, parallel);

    // Cleanup
    intlist_free(&left);
    intlist_free(&right);
    intlist_free(&sortedLeft);
    intlist_free(&sortedRight);

    return result;
}

/**
 * Function that creates a constant-weight word.
 *
 * @note The word is created as a list of integers.
 *
 * @param numberOfZero the number of 0s in the constant-weight word.
 * @param positionOfOne the list of positions where the 1s will go.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 * @return the constant-weight word composed of the number of 0s and the position of 1s required.
 */
IntList cww_merge_after_sort_recursive(int numberOfZero, IntList *positionOfOne, short parallel) {
    /// List containing the indexes of 0s (the word currently only has 0s).
    IntList positionOfZero;
    intlist_init(&positionOfZero);
    intlist_reserve(&positionOfZero, numberOfZero);

    for (int i = 0; i < numberOfZero; ++i) {
        intlist_append(&positionOfZero, i);
    }

    /// The ordered list of positions in which to insert the 1s.
    IntList sortedPositionOfOne = cww_sort_recursive(positionOfOne, parallel);

    return cww_sort_mergebits(&positionOfZero, &sortedPositionOfOne, parallel);
}