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
)

find_package(OpenMP REQUIRED)
if(OpenMP_C_FOUND)
    target_link_libraries(djb PRIVATE OpenMP::OpenMP_C)
endif()
