cmake_minimum_required(VERSION 3.31)
project(djb C)

set(CMAKE_C_STANDARD 11)

add_executable(djb
        main.c
        insertion_series/insertionSeries.c
        insertion_series/insertionSeries.h
        utility/intList.c
        utility/intList.h
        utility/pairList.c
        utility/pairList.h
        utility/safeRealloc.c
        constant_weight_words/cww.c
        constant_weight_words/cww.h
)
