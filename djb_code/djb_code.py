#!/usr/bin/env python3

# 20240515 djb
# SPDX-License-Identifier: LicenseRef-PD-hp OR CC0-1.0 OR 0BSD OR MIT-0 OR MIT

# Conventional view:
#   "insert y at position x in array"
#   takes at most linear #operations,
#   and usually takes linear #operations.
# Consider an "insertion series":
#   a loop over i of "insert Y[i] at position X[i] in the array".
# Usually takes quadratic #operations, right?

# The point of the algorithm here:
#   essentially linear #operations
#   to apply an insertion series,
#   even without input-dependent array indexing.
# See insertionseries_merge_after_sort_recursive() below.
# Simple algorithm.
#   Could easily be a textbook exercise.
#   So far searching hasn't found a reference.
#   Does anyone know a reference?

# Algorithm is parallelizable
#   if one plugs in a parallelizable merging subroutine
#   and any of the usual parallel prefix-sum algorithms.
# Algorithm is vectorizable
#   if one plugs in vectorizable merging/prefix-sum subroutines.
# Good choices of merging subroutines:
#   fast parallel merging networks (odd-even, bitonic, etc.).
#   These also naturally avoid input-dependent array indexing.
# Algorithm can handle streams as a dynamic data structure.

# The s parameter inside the algorithm can be freely selected
#   as anything between 1 and t-1
#   without affecting the output.
# Some reasonable choices compatible with parallelism etc.:
#   Choose s as floor(t/2).
#   Choose s as ceil(t/2).
#   Choose s as largest power of 2 with s < t.
#   Choose s in light of benchmarks of merge etc.
#   For streams: use s=1, then s=2, then s=4, etc.
# Some choices that simplify the algorithm:
#   Choose s as 1.
#   Choose s as t-1.

# One application of insertion series:
# map a sequence of integers to a "constant-weight binary word".
#   Parameters: nonnegative integers m,t.
#   Input: t integers X[0],X[1],...,X[t-1]
#     with 0 <= X[0] <= m, 0 <= X[1] <= m+1, etc.
#   Output: m+t bits, exactly t being 1, exactly m being 0.
#   Specifically: output is the result of
#     starting with m bits, all being 0;
#     inserting 1 at position X[0];
#     inserting 1 at position X[1];
#     etc.
#   Alternatively: do the same for t,m
#     and exchange 0 with 1.
#     This is likely faster if t > m.

# Variant: constant-weight ternary word.
#   Generate constant-weight binary word, and
#     randomly flip signs, so some 1 bits become -1.
#   Can flip signs as input to insertion series,
#     or flip signs afterwards.
# Another variant: constant number of 1, constant number of -1.
#   This is another special case of insertion series.
#   Can flip signs as input to insertion series.
#     Or just generate binary words
#     and tweak the last recursive step to flip signs,
#     choosing s as the target number of 1 positions,
#     or choosing s as the target number of -1 positions.
# Insertion series also generalizes random permutation, random shuffling, etc.

# Some applications of constant-weight words:
# cryptosystems such as McEliece and NTRU.
#   Typically want uniform distribution of words.
#     Uniform distribution of X produces uniform distribution of words.
#   Typically words are kept secret.
#     Want to avoid secret-dependent branches and array indices.
#       The algorithm here easily achieves that.
#   Typically word will be generated from an "RNG" (or "PRNG", "DRBG", etc.).
#     Typically use rejection sampling to convert bits to uniform random X.
#     Typically take larger integers mod range to reduce rejection chance.
#       Typically replace "mod range" with: multiply by range, then shift.
#       With large enough integers, rejection chance becomes negligible.
#     Or can simply skip rejection, accepting non-uniform distribution.
#     Having X close to uniform produces word close to uniform.
#       Easy to prove bounds on divergence:
#         see, e.g., https://cr.yp.to/papers.html#divergence.
#         This handles "search" applications.
#       Easy to prove bounds on statistical distance.
#         This also handles "distinguish" applications.
#         Typically imposes more stringent closeness requirement.
#   Typically want words to be generated quickly.
#     Want to limit amount of RNG output used.
#     Want to limit amount of computation.

# ===== Let's get to the code!

# ===== Subroutines

# For parallel, vectorizable, etc.,
#   replace merge() with a merging network,
#   and replace prefixsums() with parallel prefix-sum algorithms.
# Can skip last output entry from prefixsums() in context.

def merge(L,R):
    return sorted(L+R)

def prefixsums(L):
    result = [0]
    for b in L:
        result += [result[-1]+b]
    return result

# ===== Insertion series

# insertionseries API:
#   input: starting list L (or tuple etc.)
#   input: list XY of pairs
#   output: list obtained from L
#     by inserting y at position x in L
#     for, successively, each (x,y) in XY

# insertionseries_sort API:
#   input: list XY of pairs
#   output: list of (x+adjustment,y)
#     with one entry for each (x,y) from XY
#     where x+adjustment is where insertionseries ends up putting y

# insertionseries implementations:
#   insertionseries_ref
#   insertionseries_reduce_to_case_of_empty_list
#   insertionseries_linearscan_after_sort_ref
#   insertionseries_merge_after_sort_ref
#   insertionseries_merge_after_sort_recursive

# insertionseries_sort implementations:
#   insertionseries_sort_ref
#   insertionseries_sort_recursive

# helpers:
#   insertionseries_sort_merge

def insertionseries_ref(L,XY):
    L = list(L)
    for x,y in XY:
        assert 0 <= x and x <= len(L)
        L = L[:x]+[y]+L[x:]
    return L

def insertionseries_reduce_to_case_of_empty_list(L,XY):
    XY = list(enumerate(L))+list(XY)
    return insertionseries_ref([],XY)

def insertionseries_sort_ref(XY):
    L = []
    for x,y in XY:
        L = ( [(u,v) for u,v in L if u<x]
              + [(x,y)]
              + [(u+1,v) for u,v in L if x<=u]
              )
    return L

def insertionseries_linearscan_after_sort_ref(L,XY):
    L = list(L)
    R = insertionseries_sort_ref(XY)
    result = []
    for x,y in R:
        segment,L = L[:x-len(result)],L[x-len(result):]
        result += segment+[y]
    return result+L

def insertionseries_sort_merge(L,R):
    L = [(x,  1,0,y) for j,(x,y) in enumerate(L)]
    R = [(x-j,0,j,y) for j,(x,y) in enumerate(R)]
    M = merge(L,R)
    offsets = prefixsums(1-fromL for _,fromL,_,_ in M)
    return [(x+offset,y) for (x,_,_,y),offset in zip(M,offsets)]

def insertionseries_merge_after_sort_ref(L,XY):
    L = list(enumerate(L))
    R = insertionseries_sort_ref(XY)
    return [y for x,y in insertionseries_sort_merge(L,R)]

def insertionseries_sort_recursive(XY):
    XY = list(XY)
    t = len(XY)
    if t <= 1: return XY
    s = t//2
    L = insertionseries_sort_recursive(XY[:s])
    R = insertionseries_sort_recursive(XY[s:])
    return insertionseries_sort_merge(L,R)

def insertionseries_merge_after_sort_recursive(L,XY):
    L = list(enumerate(L))
    R = insertionseries_sort_recursive(XY)
    return [y for x,y in insertionseries_sort_merge(L,R)]

insertionseries_sort = insertionseries_sort_recursive
insertionseries = insertionseries_merge_after_sort_recursive

def insertionseries_test():
    import sys
    from random import randrange

    assert insertionseries_ref([1,2,3],[(0,4),(0,5),(1,6),(6,7),(4,8),(1,9)]) == [5,9,6,4,1,8,2,3,7]

    for t in range(10):
        for m in range(10):
            loops = 1000
            print(f'insertionseries t {t} m {m} loops {loops}')
            sys.stdout.flush()

            for loop in range(loops):
                bits = randrange(100)
                L = [randrange(2**bits) for i in range(m)]
                XY = [(randrange(m+i+1),randrange(2**bits)) for i in range(t)]

                S = insertionseries_sort_ref(XY)
                assert S == insertionseries_sort_recursive(XY)
                assert S == insertionseries_sort(XY)

                T = insertionseries_ref(L,XY)
                assert T == insertionseries_reduce_to_case_of_empty_list(L,XY)
                assert T == insertionseries_linearscan_after_sort_ref(L,XY)
                assert T == insertionseries_merge_after_sort_ref(L,XY)
                assert T == insertionseries_merge_after_sort_recursive(L,XY)
                assert T == insertionseries(L,XY)

                # some consistency checks
                for x,y in S:
                    assert T[x] == y
                insertedpos = set(x for x,y in S)
                assert [y for x,y in enumerate(T) if x not in insertedpos] == L
                if t == 0:
                    assert S == []
                    assert T == L
                else:
                    S2 = insertionseries_sort(XY[:-1])
                    T2 = insertionseries(L,XY[:-1])
                    x,y = XY[-1]
                    assert S == [(u,v) for u,v in S2 if u<x] + [(x,y)] + [(u+1,v) for u,v in S2 if x<=u]
                    assert T == T2[:x]+[y]+T2[x:]

# ===== Constant-weight words

# cww API:
#   input: nonnegative integer m
#   input: list X of integers
#     with 0 <= X[0] <= m; 0 <= X[1] <= m+1; etc.
#   output: list obtained from [0]*m by
#     inserting 1 at position X[0],
#     inserting 1 at position X[1],
#     etc.
#     (so list of m+len(X) entries with len(X) entries 1 and m entries 0)

# cww_sort API:
#   input: list X of nonnegative integers
#   output: sorted list of len(X) indices of 1 in the cww output

# cww implementations:
#   cww_ref
#   cww_via_insertionseries
#   cww_linearscan_after_sort_ref
#   cww_merge_after_sort_ref
#   cww_merge_after_sort_recursive

# cww_sort implementations:
#   cww_sort_ref
#   cww_sort_via_insertionseries_sort
#   cww_sort_recursive

# helpers:
#   cww_sort_mergepos
#   cww_sort_mergebits

def cww_ref(m,X):
    L = [0]*m
    for x in X:
        assert 0 <= x and x <= len(L)
        L = L[:x]+[1]+L[x:]
    return L

def cww_via_insertionseries(m,X):
    return insertionseries([0]*m,((x,1) for x in X))

def cww_sort_ref(X):
    L = []
    for x in X:
        L = ( [u for u in L if u<x]
              + [x]
              + [u+1 for u in L if x<=u]
              )
    return L

def cww_sort_via_insertionseries_sort(X):
    L = insertionseries_sort((x,0) for x in X)
    return [X for X,_ in L]

def cww_linearscan_after_sort_ref(m,X):
    L = cww_sort_ref(X)
    result = []
    for x in L:
        assert x >= len(result)
        m -= x-len(result)
        result += [0]*(x-len(result))
        result += [1]
    assert m >= 0
    result += [0]*m
    return result

def cww_sort_mergepos(L,R):
    L = [(x,1) for x in L]
    R = [(x-j,0) for j,x in enumerate(R)]
    M = merge(L,R)
    offsets = prefixsums(1-fromL for _,fromL in M)
    return [x+offset for (x,_),offset in zip(M,offsets)]

def cww_sort_mergebits(L,R):
    L = [(x,1) for x in L]
    R = [(x-j,0) for j,x in enumerate(R)]
    M = merge(L,R)
    return [1-fromL for _,fromL in M]

def cww_merge_after_sort_ref(m,X):
    return cww_sort_mergebits(range(m),cww_sort_ref(X))

def cww_sort_recursive(X):
    X = list(X)
    t = len(X)
    if t <= 1: return X
    s = t//2
    L = cww_sort_recursive(X[:s])
    R = cww_sort_recursive(X[s:])
    return cww_sort_mergepos(L,R)

def cww_merge_after_sort_recursive(m,X):
    return cww_sort_mergebits(range(m),cww_sort_recursive(X))

cww_sort = cww_sort_recursive
cww = cww_merge_after_sort_recursive

def cww_test():
    import sys
    from random import randrange

    maxloops = 10000

    for t in range(10):
        for m in range(10):
            Xpossibilities = 1
            for j in range(t): Xpossibilities *= m+t-j
            tfactorial = 1
            for j in range(t): tfactorial *= t-j

            allinputs = Xpossibilities <= maxloops
            loops = min(Xpossibilities,maxloops)

            print(f'cww m {m} t {t} loops {loops} Xpossibilities {Xpossibilities} allinputs {allinputs}')
            sys.stdout.flush()

            results = {}

            for loop in range(loops):
                r = loop if allinputs else randrange(Xpossibilities)
                X = []
                for j in range(t):
                    X += [r%(m+j+1)]
                    r //= m+j+1

                S = cww_sort_ref(X)
                assert S == cww_sort_via_insertionseries_sort(X)
                assert S == cww_sort_recursive(X)
                assert S == cww_sort(X)

                assert len(S) == t
                assert S == sorted(set(S))
                if t > 0:
                    assert S[0] >= 0
                    assert S[t-1] < m+t

                W = cww_ref(m,X)
                assert W == cww_via_insertionseries(m,X)
                assert W == cww_linearscan_after_sort_ref(m,X)
                assert W == cww_merge_after_sort_ref(m,X)
                assert W == cww_merge_after_sort_recursive(m,X)
                assert W == cww(m,X)

                assert len(W) == m+t
                assert all(y in (0,1) for y in W)
                assert sum(W) == t
                assert S == [x for x,y in enumerate(W) if y == 1]

                if allinputs:
                    S = tuple(S)
                    if S not in results:
                        results[S] = 0
                    results[S] += 1

            if allinputs:
                assert len(results)*tfactorial == loops
                for result in results:
                    assert results[result] == tfactorial

if __name__ == '__main__':
    insertionseries_test()
    cww_test()
