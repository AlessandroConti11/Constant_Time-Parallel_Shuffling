#include "intList.h"


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



/**
 * Initialize the intList list.
 *
 * @param list the intList to initialize.
 */
static void intlist_init(IntList *list) {
    list->list = NULL;
    list->listSize = list->listCapacity=0;
}

/**
 * This function ensures that the internal capacity of the intList is at least minCapacity.
 *
 * @note the capacity is always increased to powers of two for efficiency.
 *
 * @param list the intList.
 * @param minimumCapacity the minimum capacity required.
 */
static void intlist_reserve(IntList *list, size_t minimumCapacity) {
    if(minimumCapacity > list->listCapacity){
        /// The list capacity.
        size_t capacity = list->listCapacity ? list->listCapacity : 1;

        while(capacity < minimumCapacity) {
            capacity <<= 1;
        }

        list->list = safeRealloc(list->list, capacity * sizeof *list->list); list->listCapacity=capacity;
    }
}

/**
 * Function that append a new element in the list.
 *
 * @param list the intList.
 * @param element the element to append.
 */
static void intlist_append(IntList *list, int element) {
    intlist_reserve(list, list->listSize + 1);

    list->list[list->listSize++] = element;
}

/**
 * Function that insert a new element in a specific position in the list.
 *
 * @param list the intList.
 * @param position the position where the element must be inserted.
 * @param element the element to insert.
 */
static void intlist_insert(IntList *list, size_t position, int element) {
    assert(position <= list->listSize);

    intlist_reserve(list, list->listSize + 1);
    memmove(list->list + position + 1, list->list + position, (list->listSize - position) * sizeof*list->list);

    list->list[position] = element;
    ++list->listSize;
}

/**
 * function that copies the source intList to a destination.
 *
 * @param listDestination the destination intList.
 * @param listSource the source intList.
 */
static void intlist_copy(IntList *listDestination, const IntList *listSource) {
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
static void intlist_free(IntList *list) {
    free(list->list);
    intlist_init(list);
}