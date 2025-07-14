#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "utility/tuple.h"
#include "utility/intList.h"
#include "utility/pairList.h"


/**
 * Function that computes the cumulative prefixes of an intList.
 *
 * @note The first element is always 0.
 *
 * @param list the intList.
 * @return the new list that has size input list + 1, where all element is the sum of all previous elements of the input list.
 */
IntList prefixSum(const IntList *list) {
    /// The result.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, list->listSize + 1);

    /// The partial sum of each element.
    int sum = 0;

    intlist_append(&result, 0);
    for(size_t i=0; i < list->listSize; ++i) {
        sum += list->list[i];
        intlist_append(&result, sum);
    }

    return result;
}

/**
 * Function that compares two quadruple.
 *
 * @note the comparison compare the following quadruple's value:
 * 1. index0;
 * 2. fromLeft;
 * 3. indexInItsList.
 *
 * @param firstQuadruple the first quadruple to compare.
 * @param secondQuadruple the second quadruple to compare.
 * @return firstQuadruple negative value if the first quadruple is less than the first. 0 if the two quadruples are equal. A positive value if the first quadruple is greater.
 */
int quadrupleComparison(const void *firstQuadruple, const void *secondQuadruple) {
    /// First quadruple value.
    const Quadruple *first = firstQuadruple;
    /// Second quadruple value.
    const Quadruple *second = secondQuadruple;

    if(first->index0 != second->index0) {
        return (first->index0 > second->index0) - (first->index0 < second->index0);
    }

    if(first->fromLeft != second->fromLeft) {
        return (first->fromLeft > second->fromLeft) - (first->fromLeft < second->fromLeft);
    }

    return (first->indexInItsList > second->indexInItsList) - (first->indexInItsList < second->indexInItsList);
}

/**
 * Function that performs an ordered merging of two pairLists.
 *
 * @param firstList the first pairList.
 * @param secondList the second pairList.
 * @return the ordered merged pairList.
 */
PairList insertionseries_sort_merge(const PairList *firstList, const PairList *secondList) {
    /// List size of firstList.
    size_t firstListSize = firstList->listSize;
    /// List size of secondList.
    size_t secondListSize = secondList->listSize;

    /// Array of quadruple, each quadruple value corresponds to the firstList values.
    Quadruple *firstListQuadrupleArray = malloc(firstListSize * sizeof * firstListQuadrupleArray);
    /// Array of quadruple, each quadruple value corresponds to the secondList values.
    Quadruple *secondListQuadrupleArray = malloc(secondListSize * sizeof * secondListQuadrupleArray);

    for(size_t j = 0; j < firstListSize; ++j){
        firstListQuadrupleArray[j].index0 = firstList->list[j].index0;
        firstListQuadrupleArray[j].fromLeft = 1;
        firstListQuadrupleArray[j].indexInItsList = 0;
        firstListQuadrupleArray[j].index1 = firstList->list[j].index1;
    }
    // to the second list we want to give it more importance (they are the tuples not yet entered)
    for(size_t j = 0; j < secondListSize; ++j){
        secondListQuadrupleArray[j].index0 = secondList->list[j].index0 - (int)j;
        secondListQuadrupleArray[j].fromLeft = 0;
        secondListQuadrupleArray[j].indexInItsList = (int)j;
        secondListQuadrupleArray[j].index1 = secondList->list[j].index1;
    }

    /// The size of the new list of quadruple.
    size_t newQuadrupleArraySize = firstListSize + secondListSize;
    /// The new array of quadruple that contains the quadruple of the first and the second input list.
    Quadruple *newQuadrupleArray = malloc(newQuadrupleArraySize * sizeof * newQuadrupleArray);

    // merge(L, R) = copy the first; copy the second; sort
    memcpy(newQuadrupleArray, firstListQuadrupleArray, firstListSize * sizeof*newQuadrupleArray);
    memcpy(newQuadrupleArray + firstListSize, secondListQuadrupleArray, secondListSize * sizeof*newQuadrupleArray);
    qsort(newQuadrupleArray, newQuadrupleArraySize, sizeof * newQuadrupleArray, quadrupleComparison);

    // let us compute the correct offsetList to add to newQuadrupleArray.index0
    /// IntList that contains the inverse of newQuadrupleArray.fromLeft.
    IntList fromLeftInverse;
    intlist_init(&fromLeftInverse);
    intlist_reserve(&fromLeftInverse, newQuadrupleArraySize);

    for(size_t i=0; i < newQuadrupleArraySize; ++i) {
        intlist_append(&fromLeftInverse, 1 - newQuadrupleArray[i].fromLeft);
    }

    /// The list of true offset to add at each element of newQuadrupleArray.index0.
    IntList offsetList = prefixSum(&fromLeftInverse);

    /* Step‑4: produce final PairList */
    /// The output pairList.
    PairList result;
    pairlist_init(&result);
    pairlist_reserve(&result, newQuadrupleArraySize);

    for(size_t i=0; i < newQuadrupleArraySize; ++i) {
        pairlist_append(&result, newQuadrupleArray[i].index0 + offsetList.list[i], newQuadrupleArray[i].index1);
    }

    // clean the allocated list
    free(firstListQuadrupleArray);
    free(secondListQuadrupleArray);
    free(newQuadrupleArray);
    intlist_free(&fromLeftInverse);
    intlist_free(&offsetList);

    return result;
}

/**
 * Function that sorts a pairList.
 *
 * @note this function work recursively.
 *
 * @param pairList the pairList to sort.
 * @return the pairList sorted.
 */
PairList insertionseries_sort_recursive(const PairList *pairList) {
    // base case
    if(pairList->listSize <= 1){
        /// The sorted pairList.
        PairList result;

        pairlist_copy(&result, pairList);

        return result;
    }

    /// The pairList size.
    size_t pairListSize = pairList->listSize;
    /// The half of pairListSize.
    size_t halfPairListSize = pairListSize / 2;

    /// The left part of the input pairList.
    PairList left;
    pairlist_init(&left);
    pairlist_reserve(&left, halfPairListSize);
    /// The right part of the input pairList.
    PairList right;
    pairlist_init(&right);
    pairlist_reserve(&right, pairListSize - halfPairListSize);

    for(size_t i=0; i < halfPairListSize; ++i) {
        pairlist_append(&left, pairList->list[i].index0, pairList->list[i].index1);
    }
    for(size_t i=halfPairListSize; i < pairListSize; ++i) {
        pairlist_append(&right, pairList->list[i].index0, pairList->list[i].index1);
    }

    /// The left pairList sorted.
    PairList leftSort = insertionseries_sort_recursive(&left);
    /// The right pairList sorted.
    PairList rightSort = insertionseries_sort_recursive(&right);
    /// The sorted pairList.
    PairList result = insertionseries_sort_merge(&leftSort, &rightSort);

    pairlist_free(&left);
    pairlist_free(&right);
    pairlist_free(&leftSort);
    pairlist_free(&rightSort);

    return result;
}

/**
 * Function that inserts a list of values at specific positions in a list.
 *
 * @param list the intList where to insert the new values.
 * @param pairList the pairList that contains the positions and the values to insert in the intList.
 * @return the new intList with the value inserted.
 */
IntList insertionseries_merge_after_sort_recursive(const IntList *list, const PairList *pairList) {
    /// The pairList that contains all the value in the list, <actual_position, element>.
    PairList listPair;
    pairlist_init(&listPair);

    for(size_t i=0; i < list->listSize; ++i) {
        pairlist_append(&listPair, (int) i, list->list[i]);
    }

    /// The pairList sorted.
    PairList pairListSorted = insertionseries_sort_recursive(pairList);
    /// The new sorted pairList that contains listPair and pairList.
    PairList fusionSort = insertionseries_sort_merge(&listPair, &pairListSorted);

    /// The new intList with the value inserted.
    IntList result;
    intlist_init(&result);
    intlist_reserve(&result, fusionSort.listSize);

    for(size_t i=0; i < fusionSort.listSize; ++i) {
        intlist_append(&result, fusionSort.list[i].index1);
    }

    pairlist_free(&listPair);
    pairlist_free(&pairListSorted);
    pairlist_free(&fusionSort);

    return result;
}

#define insertionseries insertionseries_merge_after_sort_recursive





















/**
 * Function that insert each element in pairList in intList.
 *
 * @note Each element in pairList is a pair <insert_position, value>.
 *
 * @param intList the intList in which to insert values.
 * @param pairList the pairList that contains the position and the value to insert in the intList.
 * @return a new intList with the new value inserted.
 */
IntList insertionseries_ref(const IntList *intList, const PairList *pairList) {
    IntList newIntList;
    intlist_copy(&newIntList, intList);

    for(size_t i=0; i < pairList->listSize; ++i){
        assert(0 <= pairList->list[i].index0 && (size_t)pairList->list[i].index0 <= newIntList.listSize);

        intlist_insert(&newIntList, (size_t)pairList->list[i].index0, pairList->list[i].index1);
    }

    return newIntList;
}



/*━━━━━━━━━━ Helper: qsort for Quadruple  ━━━━━━━━━━*/


/*━━━━━━━━━━ 3. insertionseries_sort_merge(L,R)  ━━━━━━━━━━*/
/* Complete replica of the Python version, now leveraging merge() + prefixSum() */

//PairList insertionseries_sort_merge(const PairList *L,const PairList *R)
//{
//    /* Step‑1: build Quadruple lists with extra fields */
//    size_t nL=L->listSize,nR=R->listSize;
//    Quadruple *Lrec=malloc(nL * sizeof*Lrec);
//    Quadruple *Rrec=malloc(nR * sizeof*Rrec);
//    for(size_t j=0;j<nL;++j){ Lrec[j].index0=L->list[j].index0; Lrec[j].fromLeft=1; Lrec[j].indexInItsList=0; Lrec[j].index1=L->list[j].index1; }
//    for(size_t j=0;j<nR;++j){ Rrec[j].index0= R->list[j].index0 - (int)j; Rrec[j].fromLeft=0; Rrec[j].indexInItsList=(int)j; Rrec[j].index1=R->list[j].index1; }
//
//    /* Step‑2: concatenate + sort via merge() equivalent */
//    size_t n=nL+nR;
//    Quadruple *M=malloc(n * sizeof*M);
/// /    merge(L, R)
//    memcpy(M,Lrec,nL*sizeof*M);
//    memcpy(M+nL,Rrec,nR*sizeof*M);
//    qsort(M,n,sizeof* M,quadrupleComparison);
//
//    /* Step‑3: build bits list (1‑fromLeft) and compute prefix sums */
//    IntList bits; intlist_init(&bits); intlist_reserve(&bits,n);
//    for(size_t i=0;i<n;++i) intlist_append(&bits,1-M[i].fromLeft);
//    IntList offs= prefixSum(&bits); /* length n+1 */
//
//    /* Step‑4: produce final PairList */
//    PairList out; pairlist_init(&out); pairlist_reserve(&out,n);
//    for(size_t i=0;i<n;++i) pairlist_append(&out, M[i].index0 + offs.list[i], M[i].index1);
//
//    /* cleanup */
//    free(Lrec); free(Rrec); free(M); intlist_free(&bits); intlist_free(&offs);
//    return out;
//}

/*━━━━━━━━━━ 4. Recursive sort + insertionseries API  ━━━━━━━━━━*/

#define insertionseries_sort insertionseries_sort_recursive

/*– insertionseries = merge‑after‑sort‑recursive –*/


/*━━━━━━━━━━ 5. Reference implementations (for tests) ━━━━━━━━━━*/


/* (other slow reference versions omitted for brevity; keep existing fast self‑test logic) */

/*━━━━━━━━━━ 6. Self‑test  ━━━━━━━━━━*/
int randint(int max){ return rand()%max; }
void insertionseries_test(void)
{
    srand((unsigned)time(NULL));
    /* quick deterministic sanity */
    IntList L0; intlist_init(&L0); intlist_append(&L0,1); intlist_append(&L0,2); intlist_append(&L0,3);
    PairList XY0; pairlist_init(&XY0);
    pairlist_append(&XY0,0,4); pairlist_append(&XY0,0,5); pairlist_append(&XY0,1,6);
    pairlist_append(&XY0,6,7); pairlist_append(&XY0,4,8); pairlist_append(&XY0,1,9);
    IntList ref=insertionseries_ref(&L0,&XY0);
    IntList fast=insertionseries(&L0,&XY0);
    assert(ref.listSize == fast.listSize && !memcmp(ref.list, fast.list, ref.listSize * sizeof*ref.list));
    intlist_free(&ref); intlist_free(&fast);
    /* random stress */
    for(int t=0;t<10;++t) for(int m=0;m<10;++m){ int loops=500; for(int loop=0;loop<loops;++loop){ int bits=randint(20)+1; IntList L; intlist_init(&L); for(int i=0;i<m;++i) intlist_append(&L,randint(1<<bits)); PairList XY; pairlist_init(&XY); for(int i=0;i<t;++i) pairlist_append(&XY,randint(m+i+1),randint(1<<bits)); IntList T1=insertionseries_ref(&L,&XY); IntList T2=insertionseries(&L,&XY); assert(T1.listSize == T2.listSize); for(size_t k=0; k < T1.listSize; ++k) assert(T1.list[k] == T2.list[k]); intlist_free(&L); pairlist_free(&XY); intlist_free(&T1); intlist_free(&T2);} }
    puts("All tests passed.");
}

/*━━━━━━━━━━ 7. Interactive demo  ━━━━━━━━━━*/
void read_intline(IntList *L){
    char buf[4096];
    if(!fgets(buf,sizeof buf,stdin))
        return;
    char *tok=strtok(buf," \t\r\n");
    while(tok){
        intlist_append(L,atoi(tok));
        tok=strtok(NULL," \t\r\n");
    }

    printf("bonito: ");
    for (int i = 0; i < L->listSize; i++) printf("%d ", L->list[i]);
    printf("\n");
}
void read_pairs(PairList *XY){ puts("Inserisci le coppie X Y (posizione valore), una per riga;\nInvio su riga vuota per finire."); char buf[256]; while(1){ fputs("> ",stdout); fflush(stdout); if(!fgets(buf,sizeof buf,stdin)) break; if(buf[0]=='\n'||buf[0]=='\r') break; int x,y; if(sscanf(buf,"%d %d",&x,&y)==2) pairlist_append(XY,x,y); else puts("⚠️  Formato non valido: devi scrivere due interi separati da spazi."); } }

void print_intlist(const IntList *L){
    putchar('[');
    for(size_t i=0;i<L->listSize; ++i){
        if(i)
            printf(", ");
        printf("%d", L->list[i]);
    }
    puts("]");
}

int main(int argc,char **argv)
{
    puts("=== Inserimento serie di inserzioni ===");
    IntList L;
    intlist_init(&L);
    fputs("Lista iniziale L (numeri separati da spazi): ",stdout);
    fflush(stdout); read_intline(&L);

    printf("ciao: ");
    for (int i = 0; i < L.listSize; i++){ printf("%d ", L.list[i]);}
//    print_intlist(&L);

    PairList XY;
    pairlist_init(&XY);
    read_pairs(&XY);

    int bad=0;
    for(size_t i=0;i<XY.listSize; ++i)
        if(XY.list[i].index0 < 0 || (size_t)XY.list[i].index0 > L.listSize + i)
            bad=1;
    if(bad){
        puts("\n❌  Errore: qualche coppia X è fuori intervallo.");
        goto cleanup;
    }

    IntList result=insertionseries_merge_after_sort_recursive(&L,&XY);

    puts("\nRisultato dopo le inserzioni:");
    print_intlist(&result);
    PairList S=insertionseries_sort_recursive(&XY);

    puts("\nPosizioni finali (x', index1) calcolate da insertionseries_sort:");
    for(size_t i=0;i<S.listSize; ++i)
        printf("pos %d: %d\n", S.list[i].index0, S.list[i].index1);

    intlist_free(&result); pairlist_free(&S);

    cleanup: intlist_free(&L);
    pairlist_free(&XY);
    return 0;
}
