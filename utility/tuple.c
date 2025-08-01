#include "tuple.h"


/**
* Function that compares two quadruple.
 *
 * @note The comparison compares the following quadruple's value:
 * 1. index0;
 * 2. fromLeft;
 * 3. indexInItsList.
 *
 * @param firstQuadruple the first quadruple to compare.
 * @param secondQuadruple the second quadruple to compare.
 * @return firstQuadruple negative value if the first quadruple is less than the first. 0 if the two quadruples are equal. A positive value if the first quadruple is greater.
 */
int quadrupleComparison(void *firstQuadruple, const void *secondQuadruple) {
    /// First quadruple value.
    const Quadruple *first = firstQuadruple;
    /// Second quadruple value.
    const Quadruple *second = secondQuadruple;

    // constant-time
    return
        // if first->index0 != second->index0 --> return (first->index0 > second->index0) - (first->index0 < second->index0)
        (1 - (first->index0 == second->index0)) * ((first->index0 > second->index0) - (first->index0 < second->index0)) +
        // else if first->fromLeft != second->fromLeft --> return (first->fromLeft > second->fromLeft) - (first->fromLeft < second->fromLeft);
        ((first->index0 == second->index0) * (1 - (first->fromLeft == second->fromLeft))) * ((first->fromLeft > second->fromLeft) - (first->fromLeft < second->fromLeft)) +
        // else --> (first->indexInItsList > second->indexInItsList) - (first->indexInItsList < second->indexInItsList);
        ((first->index0 == second->index0) * (first->fromLeft == second->fromLeft)) * ((first->indexInItsList > second->indexInItsList) - (first->indexInItsList < second->indexInItsList));
}