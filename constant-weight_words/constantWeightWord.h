#ifndef CONSTANTWEIGHTWORD_H
#define CONSTANTWEIGHTWORD_H


#include <omp.h>
#include <stddef.h>


#include "../utility/intList.h"
#include "../utility/pairList.h"
#include "../insertion_series/insertionSeries.h"


IntList cww_via_insertionseries(int numberOfZero, IntList *positionOfOne, short parallel);


IntList cww_sort_mergebits(const IntList *positionOfZero, const IntList *positionOfOne, short parallel);
IntList cww_sort_mergepos(const IntList *firstList, const IntList *secondList, short parallel);
IntList cww_sort_recursive(const IntList *intList, short parallel);

IntList cww_merge_after_sort_recursive(int numberOfZero, IntList *positionOfOne, short parallel);


#define cww cww_merge_after_sort_recursive


#endif //CONSTANTWEIGHTWORD_H
