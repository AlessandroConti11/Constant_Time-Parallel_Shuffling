#ifndef DJB_SAFEREALLOC_H
#define DJB_SAFEREALLOC_H


#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


void *safeRealloc(void *pointerToRealloc, size_t reallocSize);


#endif //DJB_SAFEREALLOC_H
