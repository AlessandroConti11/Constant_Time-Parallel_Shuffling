#include "intList.h"


/**
 * Function that initializes the intList.
 *
 * @param list the intList to initialize.
 */
void intlist_init(IntList *list) {
    list->list = NULL;
    list->listSize = 0;
    list->listCapacity = 0;
}

/**
 * Function that ensures that the internal capacity of the intList is at least minCapacity.
 *
 * @note The capacity is always increased to powers of two for efficiency.
 *
 * @param list the intList.
 * @param minimumCapacity the minimum capacity required.
 */
void intlist_reserve(IntList *list, size_t minimumCapacity) {
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
 * @param list the intList.
 * @param element the element to append.
 */
void intlist_append(IntList *list, int element) {
    intlist_reserve(list, list->listSize + 1);

    list->list[list->listSize++] = element;
}

/**
 * Function that inserts a new element in a specific position in the intlist.
 *
 * @param list the intList.
 * @param position the position where the element must be inserted.
 * @param element the element to insert.
 */
void intlist_insert(IntList *list, size_t position, int element) {
    assert(position <= list->listSize);

    intlist_reserve(list, list->listSize + 1);
    memmove(list->list + position + 1, list->list + position, (list->listSize - position) * sizeof * list->list);

    list->list[position] = element;
    ++list->listSize;
}

/**
 * Function that copies the source intList to a destination.
 *
 * @param listDestination the destination intList.
 * @param listSource the source intList.
 */
void intlist_copy(IntList *listDestination, const IntList *listSource) {
    assert(listSource->listSize > 0);

    listDestination->listSize = listSource->listSize;
    listDestination->listCapacity = listSource->listSize;
    listDestination->list = malloc(listSource->listSize * sizeof*listSource->list);

    memcpy(listDestination->list, listSource->list, listSource->listSize * sizeof *listSource->list);
}

/**
 * Function which frees the memory allocated for the intList and resets the structure to its initial state.
 *
 * @param list the intList.
 */
void intlist_free(IntList *list) {
    free(list->list);
    intlist_init(list);
}