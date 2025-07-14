cmake_minimum_required(VERSION 3.31)
project(djb C)

set(CMAKE_C_STANDARD 11)

add_executable(djb #insertionSeries.c
        ciao.c
        insertionSeries.h
        utility/intList.c
        utility/intList.h
#        utility/tuple.h
        utility/pairList.c
        utility/pairList.h
#        utility/safeRealloc.h
)
