cmake_minimum_required(VERSION 3.3)
project(graphics)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Not portable in any way ;)
set(MINGW_PREFIX /usr/x86_64-w64-mingw32/sys-root/mingw)
set(CROSS_TOOLCHAIN x86_64-w64-mingw32-)
set(CMAKE_AR ${CROSS_TOOLCHAIN}ar)
set(CMAKE_RANLIB ${CROSS_TOOLCHAIN}ranlib)
set(CMAKE_CXX_COMPILER ${CROSS_TOOLCHAIN}g++)

set(SDL_LINK_FLAGS "-lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic --std=c++11 -Dmain=SDL_main")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SDL_LINK_FLAGS}")

include_directories(${graphics_SOURCE_DIR}/include ${MINGW_PREFIX}/include)
link_directories(${graphics_SOURCE_DIR}/lib ${MINGW_PREFIX}/lib)

add_subdirectory(lib)
add_subdirectory(lb1)
add_subdirectory(lb2)
