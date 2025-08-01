#include "pairList.h"


/**
 * Function that initializes the pairList.
 *
 * @param list the pairList to initialize.
 */
void pairlist_init(PairList *list) {
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
void pairlist_reserve(PairList *list, size_t minimumCapacity) {
    /// The previous capacity.
    size_t previousCapacity = list->listCapacity;

    // Metodo: partendo da minimumCapacity - 1 "propaghi" i bit a 1
    /// The calculated capacity.
    size_t capacity = minimumCapacity - 1;
    // compute the smallest power of 2 greater than the< minimum capacity
    // starting from the minimum capacity - 1 propagate the bits to 1
    capacity |= capacity >> 1;
    capacity |= capacity >> 2;
    capacity |= capacity >> 4;
    capacity |= capacity >> 8;
    capacity |= capacity >> 16;
    // size_t sizze
#if SIZE_MAX > UINT32_MAX
    cap |= cap >> 32;  // se size_t > 32 bit
#endif
    // capacity = cap + 1;
    capacity++;

    /// The realloc is necessarily or not.
    size_t isReallocNeeded = (minimumCapacity > previousCapacity);
    /// The mux selector.
    /// @details minimumCapacity > previousCapacity --> -1 = 0xFF...FF
    /// @details minimumCapacity < previousCapacity --> 0  = 0x00...00
    size_t muxSelector = (size_t)-isReallocNeeded;

    /// The new capacity
    size_t newCapacity = (capacity & muxSelector) | (previousCapacity & ~muxSelector);

    list->list = (isReallocNeeded) ? safeRealloc(list->list, newCapacity * sizeof *list->list) : list->list;

    list->listCapacity = (newCapacity & muxSelector) | (previousCapacity & ~muxSelector);
}

/**
 * Function that appends a new element in the list.
 *
 * @param list the pairList.
 * @param elementIndex0 the index0 of the element to append.
 * @param elementIndex1 the index1 of the element to append.
 */
void pairlist_append(PairList *list, int elementIndex0, int elementIndex1) {
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
void pairlist_insert(PairList *list, size_t position, int elementIndex0, int elementIndex1) {
    assert(position <= list->listSize);

    pairlist_reserve(list, list->listSize + 1);
    memmove(list->list + position + 1, list->list + position, (list->listSize - position) * sizeof * list->list);

    list->list[position].index0 = elementIndex0;
    list->list[position].index1 = elementIndex1;
    ++list->listSize;
}

/**
 * Function that copies the source pairList to a destination.
 *
 * @param listDestination the destination pairList.
 * @param listSource the source pairList.
 */
void pairlist_copy(PairList *listDestination, const PairList *listSource) {
    assert(listSource->listSize > 0);

    listDestination->listSize = listSource->listSize;
    listDestination->listCapacity = listSource->listSize;
    listDestination->list = malloc(listSource->listSize * sizeof * listSource->list);

    memcpy(listDestination->list, listSource->list, listSource->listSize * sizeof *listSource->list);
}

/**
 * Function that frees the memory allocated for the pairList and resets the structure to its initial state.
 *
 * @param list the pairList.
 */
void pairlist_free(PairList *list) {
    free(list->list);
    pairlist_init(list);
}