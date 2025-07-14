#include "pairList.h"
#include "intList.h"


/**
 * Function that initializes the pairList.
 *
 * @param list the pairList to initialize.
 */
static void pairlist_init(PairList *list) {
    list->list = NULL;
    list->listSize = 0;
    list->listCapacity = 0;
}

/**
 * Function that ensures that the internal capacity of the pairList is at least minCapacity.
 *
 * @note the capacity is always increased to powers of two for efficiency.
 *
 * @param list the pairList.
 * @param minimumCapacity the minimum capacity required.
 */
static void pairlist_reserve(PairList *list, size_t minimumCapacity) {
    if(minimumCapacity > list->listCapacity) {
        size_t c = list->listCapacity ? list->listCapacity : 1;

        while(c < minimumCapacity) {
            c<<=1;
        }

        list->list = safeRealloc(list->list, c * sizeof *list->list);
        list->listCapacity = c;
    }
}

/**
 * Function that appends a new element in the list.
 *
 * @param list the pairList.
 * @param elementIndex0 the index0 of the element to append.
 * @param elementIndex1 the index1 of the element to append.
 */
static void pairlist_append(PairList *list, int elementIndex0, int elementIndex1) {
    pairlist_reserve(list, list->listSize + 1);

    list->list[list->listSize].index0 = elementIndex0;
    list->list[list->listSize].index1 = elementIndex1;

    ++list->listSize;
}

/**
 * Function that inserts a new element in a specific position in the pairList.
 *
 * @param list the pairList.
 * @param position the position where the element must be inserted.
 * @param elementIndex0 the index0 of the element to append.
 * @param elementIndex1 the index1 of the element to append.
 */
static void pairlist_insert(PairList *list, size_t position, int elementIndex0, int elementIndex1) {
    assert(position <= list->listSize);

    pairlist_reserve(list, list->listSize + 1);
    memmove(list->list + position + 1, list->list + position, (list->listSize - position) * sizeof*list->list);

    list->list[position].index0=elementIndex0;
    list->list[position].index1=elementIndex1;
    ++list->listSize;
}

/**
 * Function that copies the source pairList to a destination.
 *
 * @param listDestination the destination pairList.
 * @param listSource the source pairList.
 */
static void pairlist_copy(PairList *listDestination, const PairList *listSource) {
    listDestination->listSize=listSource->listSize;
    listDestination->listCapacity=listSource->listSize;
    listDestination->list= listSource->listSize ? malloc(listSource->listSize * sizeof*listSource->list) : NULL;

    if(listSource->listSize) {
        memcpy(listDestination->list, listSource->list, listSource->listSize * sizeof *listSource->list);
    }
}

/**
 * Function that frees the memory allocated for the intList and resets the structure to its initial state.
 *
 * @param list the pairList.
 */
static void pairlist_free(PairList *list) {
    free(list->list);
    pairlist_init(list);
}