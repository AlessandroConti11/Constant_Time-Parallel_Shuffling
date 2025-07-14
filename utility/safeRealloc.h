#ifndef DJB_SAFEREALLOC_H
#define DJB_SAFEREALLOC_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>


void *safeRealloc(void *pointerToRealloc, size_t reallocSize);


#endif //DJB_SAFEREALLOC_H
