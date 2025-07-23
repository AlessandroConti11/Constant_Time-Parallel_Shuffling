#ifndef DJB_INSERTIONSERIES_H
#define DJB_INSERTIONSERIES_H


#include <omp.h>
#include <stddef.h>

#include "../utility/tuple.h"
#include "../utility/intList.h"
#include "../utility/pairList.h"


int quadrupleComparison(const void *firstQuadruple, const void *secondQuadruple);
size_t binarySearch(Quadruple *list, size_t start, size_t end, Quadruple key);

IntList prefixSum(const IntList *list, short parallel);
IntList prefixSumSerial(const IntList *list);
IntList prefixSumParallel(const IntList *list);

Quadruple *merge(Quadruple *firstList, size_t firstListSize, Quadruple *secondList, size_t secondListSize, short parallel);
Quadruple *mergeSerial(Quadruple *firstList, size_t firstListSize, Quadruple *secondList, size_t secondListSize);
Quadruple *mergeParallel(Quadruple *firstList, size_t firstListSize, Quadruple *secondList, size_t secondListSize);

PairList insertionseries_sort_merge(const PairList *firstList, const PairList *secondList, short parallel);
PairList insertionseries_sort_recursive(const PairList *pairList, short parallel);
IntList insertionseries_merge_after_sort_recursive(const IntList *list, const PairList *pairList, short parallel);


#define insertionseries insertionseries_merge_after_sort_recursive


#endif //DJB_INSERTIONSERIES_H
