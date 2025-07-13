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

def merge(L, R):
    return sorted(L + R)


def prefixsums(L):
    result = [0]
    for b in L:
        result += [result[-1] + b]
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

def insertionseries_ref(L, XY):
    L = list(L)
    for x, y in XY:
        assert 0 <= x and x <= len(L)
        L = L[:x] + [y] + L[x:]
    return L


def insertionseries_reduce_to_case_of_empty_list(L, XY):
    XY = list(enumerate(L)) + list(XY)
    return insertionseries_ref([], XY)


def insertionseries_sort_ref(XY):
    L = []
    for x, y in XY:
        L = ([(u, v) for u, v in L if u < x]
             + [(x, y)]
             + [(u + 1, v) for u, v in L if x <= u]
             )
    return L


def insertionseries_linearscan_after_sort_ref(L, XY):
    L = list(L)
    R = insertionseries_sort_ref(XY)
    result = []
    for x, y in R:
        segment, L = L[:x - len(result)], L[x - len(result):]
        result += segment + [y]
    return result + L


def insertionseries_sort_merge(L, R):
    L = [(x, 1, 0, y) for j, (x, y) in enumerate(L)]
    R = [(x - j, 0, j, y) for j, (x, y) in enumerate(R)]
    M = merge(L, R)
    offsets = prefixsums(1 - fromL for _, fromL, _, _ in M)
    return [(x + offset, y) for (x, _, _, y), offset in zip(M, offsets)]


def insertionseries_merge_after_sort_ref(L, XY):
    L = list(enumerate(L))
    R = insertionseries_sort_ref(XY)
    return [y for x, y in insertionseries_sort_merge(L, R)]


def insertionseries_sort_recursive(XY):
    XY = list(XY)
    t = len(XY)
    if t <= 1: return XY
    s = t // 2
    L = insertionseries_sort_recursive(XY[:s])
    R = insertionseries_sort_recursive(XY[s:])
    return insertionseries_sort_merge(L, R)


def insertionseries_merge_after_sort_recursive(L, XY):
    L = list(enumerate(L))
    R = insertionseries_sort_recursive(XY)
    return [y for x, y in insertionseries_sort_merge(L, R)]


insertionseries_sort = insertionseries_sort_recursive
insertionseries = insertionseries_merge_after_sort_recursive


def insertionseries_test():
    import sys
    from random import randrange

    assert insertionseries_ref([1, 2, 3], [(0, 4), (0, 5), (1, 6), (6, 7), (4, 8), (1, 9)]) == [5, 9, 6, 4, 1, 8, 2, 3,
                                                                                                7]

    for t in range(10):
        for m in range(10):
            loops = 1000
            print(f'insertionseries t {t} m {m} loops {loops}')
            sys.stdout.flush()

            for loop in range(loops):
                bits = randrange(100)
                L = [randrange(2 ** bits) for i in range(m)]
                XY = [(randrange(m + i + 1), randrange(2 ** bits)) for i in range(t)]

                S = insertionseries_sort_ref(XY)
                assert S == insertionseries_sort_recursive(XY)
                assert S == insertionseries_sort(XY)

                T = insertionseries_ref(L, XY)
                assert T == insertionseries_reduce_to_case_of_empty_list(L, XY)
                assert T == insertionseries_linearscan_after_sort_ref(L, XY)
                assert T == insertionseries_merge_after_sort_ref(L, XY)
                assert T == insertionseries_merge_after_sort_recursive(L, XY)
                assert T == insertionseries(L, XY)

                # some consistency checks
                for x, y in S:
                    assert T[x] == y
                insertedpos = set(x for x, y in S)
                assert [y for x, y in enumerate(T) if x not in insertedpos] == L
                if t == 0:
                    assert S == []
                    assert T == L
                else:
                    S2 = insertionseries_sort(XY[:-1])
                    T2 = insertionseries(L, XY[:-1])
                    x, y = XY[-1]
                    assert S == [(u, v) for u, v in S2 if u < x] + [(x, y)] + [(u + 1, v) for u, v in S2 if x <= u]
                    assert T == T2[:x] + [y] + T2[x:]



# ===== Main interattivo =========================================
#
# Come si usa:
#   $ python3 insertionseries.py
#
# 1. Inserisci gli elementi di L separati da spazi (es. "10 20 30").
# 2. Inserisci le coppie X Y una per riga (es. "0 99"), premi Invio a
#    riga vuota per terminare.
# 3. Il programma stampa:
#      • la lista risultante dopo tutte le inserzioni
#      • la lista (x',y) restituita da insertionseries_sort (facoltativo)

def _read_int_list(prompt):
    """Ritorna una lista di interi letti da input()."""
    line = input(prompt).strip()
    if not line:
        return []
    return [int(tok) for tok in line.split()]

def _read_pairs():
    """Legge righe 'x y' finché l’utente dà Invio a riga vuota."""
    pairs = []
    print("Inserisci le coppie X Y (posizione valore), una per riga; "
          "Invio su riga vuota per finire.")
    while True:
        line = input("> ").strip()
        if line == "":
            break
        try:
            x_str, y_str = line.split()
            pairs.append((int(x_str), int(y_str)))
        except ValueError:
            print("⚠️  Formato non valido: devi scrivere due interi separati da spazi.")
    return pairs

def main():
    print("=== Inserimento serie di inserzioni ===")
    L = _read_int_list("Lista iniziale L (numeri separati da spazi): ")
    XY = _read_pairs()

    try:
        result = insertionseries(L, XY)
    except AssertionError:
        print("\n❌  Errore: qualche coppia X è fuori intervallo.")
        return

    print("\nRisultato dopo le inserzioni:")
    print(result)

    # Facoltativo: mostra anche le posizioni finali calcolate da insertionseries_sort
    S = insertionseries_sort(XY)
    print("\nPosizioni finali (x', y) calcolate da insertionseries_sort:")
    for x, y in S:
        print(f"pos {x}: {y}")


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(
        description="Esegue la serie di inserzioni oppure i test interni."
    )
    parser.add_argument(
        '--test',
        action='store_true',
        help="Esegue insertionseries_test() invece del main interattivo"
    )
    args = parser.parse_args()

    if args.test:
        insertionseries_test()
    else:
        main()


