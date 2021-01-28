cmake_minimum_required(VERSION 3.17)
project(minic C)

set(CMAKE_C_STANDARD 11)

add_executable(codes main.c format.c scan.c) # 这儿可能要加 .h