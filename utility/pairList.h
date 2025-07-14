#ifndef DJB_PAIRLIST_H
#define DJB_PAIRLIST_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "tuple.h"
#include "safeRealloc.h"


typedef struct {
    Pair *list;
    size_t listSize;
    size_t listCapacity;
} PairList;


void pairlist_init(PairList *list);
void pairlist_reserve(PairList *list, size_t minimumCapacity);
void pairlist_append(PairList *list, int elementIndex0, int elementIndex1);
void pairlist_insert(PairList *list, size_t position, int elementIndex0, int elementIndex1);
void pairlist_copy(PairList *listDestination, const PairList *listSource);
void pairlist_free(PairList *list);


#endif //DJB_PAIRLIST_H
