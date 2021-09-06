cmake_minimum_required(VERSION 3.20)
project(Hochsprachen_Compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(Hochsprachen_Compiler main.c helper.c helper.h)
