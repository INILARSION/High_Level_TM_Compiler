cmake_minimum_required(VERSION 3.20)
project(Hochsprachen_Compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(Hochsprachen_Compiler main.c helper.c helper.h math_ops.c math_ops.h cmp_ops.c cmp_ops.h jump_ops.c jump_ops.h)
