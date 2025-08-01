#include "bitonicSort.h"


/**
 * The sort algorithm of bitonic sort.
 *
 * @details This algorithm is an adaptation of the original algorithm, which only works with arrays whose size is a power of 2.
 *
 * @param array the unsorted array.
 * @param startPosition the starting position.
 * @param arraySize the array size.
 * @param direction the sorting direction.
 * @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 */
void bitonicSort(Quadruple *array, size_t startPosition, size_t arraySize, short direction, short parallel) {
    if (arraySize > 1) {
        /// The subarray size.
        size_t subarraySize = arraySize / 2;

        if (parallel) {
#pragma omp parallel sections
            {
#pragma omp section
                {
                    bitonicSort(array, startPosition, subarraySize, !direction, parallel);
                }

#pragma omp section
                {
                    bitonicSort(array, startPosition + subarraySize, arraySize - subarraySize, direction, parallel);
                }
            }
        }
        else {
            bitonicSort(array, startPosition, subarraySize, !direction, parallel);
            bitonicSort(array, startPosition + subarraySize, arraySize - subarraySize, direction, parallel);
        }

        bitonicMerge(array, startPosition, arraySize, direction, parallel);
    }
}

/**
 * The merge algorithm of adapted bitonic sort.
 *
 * @param array the unsorted array.
 * @param startPosition the starting position.
 * @param arraySize the array size.
 * @param direction the sorting direction.
* @param parallel the type of algorithm execution, either parallel mode, 1, or serial mode, 0.
 */
void bitonicMerge(Quadruple *array, size_t startPosition, size_t arraySize, short direction, short parallel) {
    if (arraySize > 1) {
        /// The subarray size.
        size_t subarraySize = greatestPowerOf2LessThan(arraySize);

        if (parallel) {
#pragma omp parallel for
            for (size_t i = startPosition; i < startPosition + arraySize - subarraySize; ++i) {
                compareAndSwap(&array[i], &array[i + subarraySize], direction);
            }
        }
        else {
            for (size_t i = startPosition; i < startPosition + arraySize - subarraySize; ++i) {
                compareAndSwap(&array[i], &array[i + subarraySize], direction);
            }
        }

        bitonicMerge(array, startPosition, subarraySize, direction, parallel);
        bitonicMerge(array, startPosition + subarraySize, arraySize - subarraySize, direction, parallel);
    }
}


/**
 * Function that compares and swaps two elements based on the sorting direction.
 *
 * @param firstElement the first element.
 * @param secondElement the second element.
 * @param direction the sorting direction.
 */
void compareAndSwap(Quadruple *firstElement, Quadruple *secondElement, short direction) {
    /// The mux selector.
    /// @details start + chunk < listSize --> -1 = 0xFF...FF
    /// @details start + chunk > listSize --> 0  = 0x00...00
    unsigned int muxSelector = (unsigned int)-(direction == (quadrupleComparison(firstElement, secondElement) > 0));

    /// The temporary variable used for the swap.
    Quadruple temp;

    temp.index0      = (firstElement->index0 & ~muxSelector) | (secondElement->index0 & muxSelector);
    temp.fromLeft    = (firstElement->fromLeft & ~muxSelector) | (secondElement->fromLeft & muxSelector);
    temp.indexInItsList = (firstElement->indexInItsList & ~muxSelector) | (secondElement->indexInItsList & muxSelector);
    temp.index1      = (firstElement->index1 & ~muxSelector) | (secondElement->index1 & muxSelector);

    secondElement->index0      = (secondElement->index0 & ~muxSelector) | (firstElement->index0 & muxSelector);
    secondElement->fromLeft    = (secondElement->fromLeft & ~muxSelector) | (firstElement->fromLeft & muxSelector);
    secondElement->indexInItsList = (secondElement->indexInItsList & ~muxSelector) | (firstElement->indexInItsList & muxSelector);
    secondElement->index1      = (secondElement->index1 & ~muxSelector) | (firstElement->index1 & muxSelector);

    firstElement->index0      = temp.index0;
    firstElement->fromLeft    = temp.fromLeft;
    firstElement->indexInItsList = temp.indexInItsList;
    firstElement->index1      = temp.index1;
}

/**
 * Function that finds the greatest power of two that is less than or equal to a given number.
 *
 * @param n the input number.
 * @return the greatest power of two less than or equal to the input number.
 */
int greatestPowerOf2LessThan(const int n) {
    /// The gratest power of 2 less than or equal to the input number.
    int result = n;

    result = result - 1;
    result |= result >> 1;
    result |= result >> 2;
    result |= result >> 4;
    result |= result >> 8;
    result |= result >> 16;

    return (result + 1) >> 1;
}



