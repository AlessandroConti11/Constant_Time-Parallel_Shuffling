#ifndef DJB_PAIRLIST_H
#define DJB_PAIRLIST_H


#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "tuple.h"
#include "safeRealloc.h"


/// The new type representing a list of pair.
typedef struct {
    /// The list.
    Pair *list;
    /// The current size of the list.
    size_t listSize;
    /// The maximum capacity of the list.
    size_t listCapacity;
} PairList;


void pairlist_init(PairList *list);
void pairlist_reserve(PairList *list, size_t minimumCapacity);
void pairlist_append(PairList *list, int elementIndex0, int elementIndex1);
void pairlist_insert(PairList *list, size_t position, int elementIndex0, int elementIndex1);
void pairlist_copy(PairList *listDestination, const PairList *listSource);
void pairlist_free(PairList *list);


#endif //DJB_PAIRLIST_H
