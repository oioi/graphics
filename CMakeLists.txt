cmake_minimum_required(VERSION 3.3)
project(graphics)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic --std=c++11")

include_directories(${graphics_SOURCE_DIR}/include)
link_directories(${graphics_SOURCE_DIR}/lib /lib/sdl2)

add_subdirectory(lib)
add_subdirectory(lb1)
add_subdirectory(lb2)
