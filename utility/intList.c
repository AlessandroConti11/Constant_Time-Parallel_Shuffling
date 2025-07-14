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
    if(minimumCapacity > list->listCapacity){
        /// The list capacity.
        size_t capacity = list->listCapacity ? list->listCapacity : 1;

        while(capacity < minimumCapacity) {
            capacity <<= 1;
        }

        list->list = safeRealloc(list->list, capacity * sizeof *list->list);
        list->listCapacity = capacity;
    }
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
    listDestination->listSize = listSource->listSize;
    listDestination->listCapacity = listSource->listSize;
    listDestination->list = listSource->listSize ? malloc(listSource->listSize * sizeof*listSource->list) : NULL;

    if(listSource->listSize) {
        memcpy(listDestination->list, listSource->list, listSource->listSize * sizeof *listSource->list);
    }
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