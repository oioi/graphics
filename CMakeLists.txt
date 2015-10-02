cmake_minimum_required(VERSION 3.3)
project(graphics)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic --std=c++11 -mwindows -mconsole")

include_directories(${graphics_SOURCE_DIR/}include)
link_directories(/lib/sdl2)

add_subdirectory(lib)
add_subdirectory(lb1)
