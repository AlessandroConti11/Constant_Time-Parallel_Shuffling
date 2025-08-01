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





void compareAndSwap(Quadruple *a, Quadruple *b, short dir) {
    if (dir == (quadrupleComparison(a, b) > 0)) {
        Quadruple temp = *a;
        *a = *b;
        *b = temp;
    }
}



int greatestPowerOf2LessThan(const int n) {
    ///The greatest power of two less or equal than the input number.
    int k = 1;

    while (k > 0 && k < n) {
        k = k << 1;
    }

    return k >> 1;
}



