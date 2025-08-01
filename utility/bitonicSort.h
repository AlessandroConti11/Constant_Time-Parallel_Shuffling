#ifndef BITONICSORT_H
#define BITONICSORT_H


#include <omp.h>
#include <stddef.h>

#include "tuple.h"


#define ASCENDING 1
#define DESCENDING 0


void bitonicSort(Quadruple *array, size_t startPosition, size_t arraySize, short direction, short parallel);

void bitonicMerge(Quadruple *array, size_t startPosition, size_t arraySize, short direaction, short parallel);

int greatestPowerOf2LessThan(const int n);
void compareAndSwap(Quadruple *a, Quadruple *b, short dir);


#endif //BITONICSORT_H
