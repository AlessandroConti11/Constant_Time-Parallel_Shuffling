#ifndef DJB_INTLIST_H
#define DJB_INTLIST_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "safeRealloc.h"


typedef struct {
    int *list;
    size_t listSize;
    size_t listCapacity;
} IntList;


void intlist_init(IntList *list);
void intlist_reserve(IntList *list, size_t minimumCapacity);
void intlist_append(IntList *list, int element);
void intlist_insert(IntList *list, size_t position, int element);
void intlist_copy(IntList *listDestination, const IntList *listSource);
void intlist_free(IntList *list);


#endif //DJB_INTLIST_H
