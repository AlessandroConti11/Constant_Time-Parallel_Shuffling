#ifndef DJB_INTLIST_H
#define DJB_INTLIST_H


#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "safeRealloc.h"


/// The new type representing a list of integer.
typedef struct {
    /// The list.
    int *list;
    /// The current size of the list.
    size_t listSize;
    /// The maximum capacity of the list.
    size_t listCapacity;
} IntList;


void intlist_init(IntList *list);
void intlist_reserve(IntList *list, size_t minimumCapacity);
void intlist_append(IntList *list, int element);
void intlist_insert(IntList *list, size_t position, int element);
void intlist_copy(IntList *listDestination, const IntList *listSource);
void intlist_free(IntList *list);


#endif //DJB_INTLIST_H
