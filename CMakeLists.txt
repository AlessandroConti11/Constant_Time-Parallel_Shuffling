cmake_minimum_required(VERSION 3.31)
project(djb C)

set(CMAKE_C_STANDARD 11)

add_library(djb STATIC insertionSeries.c
        ciao.c
        utility/intList.c
        utility/intList.h)
