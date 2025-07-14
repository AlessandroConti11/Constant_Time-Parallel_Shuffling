#ifndef DJB_SAFEREALLOC_H
#define DJB_SAFEREALLOC_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>


/**
 * New realloc version.
 *
 * @note If reallocSize is 0, this function allocates 1 byte instead of zero, to avoid problems with realloc implementations that may return NULL.
 *
 * @param pointerToRealloc the pointer to memory block to be reallocate.
 * @param reallocSize the new memory block size.
 * @return the pointer to the new memory block.
 */
static void *safeRealloc(void *pointerToRealloc, size_t reallocSize) {
    /// New pointer.
    void *newPointer = realloc(pointerToRealloc, reallocSize ? reallocSize : 1);


    if (!newPointer) {
        perror("realloc"); exit(EXIT_FAILURE);
    }

    return newPointer;
}


#endif //DJB_SAFEREALLOC_H
