cmake_minimum_required(VERSION 3.0.0)
project(midterm_practise)

IF(CMAKE_COMPILER_IS_GNUCC)
MESSAGE(STATUS "=== GCC C COMPILER DETECTED")
SET(CMAKE_C_FLAGS "-m32 -ggdb -D_DEBUG -O0 -D_LARGEFILE64_SOURCE")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

add_executable(midterm_practise main.c)
TARGET_LINK_LIBRARIES(midterm_practise pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)