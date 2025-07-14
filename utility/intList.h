#ifndef DJB_INTLIST_H
#define DJB_INTLIST_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>


typedef struct {
    int *list;
    size_t listSize;
    size_t listCapacity;
} IntList;


static void *safeRealloc(void *pointerToRealloc, size_t reallocSize);

static void intlist_init(IntList *list);
static void intlist_reserve(IntList *list, size_t minimumCapacity);
static void intlist_append(IntList *list, int element);
static void intlist_insert(IntList *list, size_t position, int element);
static void intlist_copy(IntList *listDestination, const IntList *listSource);
static void intlist_free(IntList *list);


#endif //DJB_INTLIST_H
