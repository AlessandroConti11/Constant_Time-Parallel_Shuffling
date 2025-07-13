cmake_minimum_required(VERSION 3.31)
project(cryptography C)

set(CMAKE_C_STANDARD 11)

add_library(cryptography STATIC library.c)
