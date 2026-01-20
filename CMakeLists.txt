cmake_minimum_required(VERSION 3.29)
project(untitled C)

set(CMAKE_C_STANDARD 11)

add_library(untitled STATIC library.c
        Main.c
)
cmake_minimum_required(VERSION 3.16)
project(FiniteAutomataProject)

set(CMAKE_C_STANDARD 99)

add_executable(FiniteAutomata Main.c library.c)
