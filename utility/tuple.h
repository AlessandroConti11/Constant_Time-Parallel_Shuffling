#ifndef DJB_TUPLE_H
#define DJB_TUPLE_H


/// The new type representing a pair <index0, index1>.
typedef struct {
    /// The first value of the pair.
    int index1;
    /// The second value of the pair.
    int index0;
} Pair;


/// The new type representing a quadruple < index0, fromLeft, indexInItsList, index1>.
typedef struct {
    /// The first value of the quadruple.
    int index0;
    /// The values of the quadruple are either the left pair, 1, or the right pair, 0.
    int fromLeft;
    /// The index in the list of source pairs.
    int indexInItsList;
    /// The second value of the quadruple.
    int index1;
} Quadruple;


#endif //DJB_TUPLE_H
