set(SOURCES main.cpp)
add_executable(lb2 ${SOURCES})
target_link_libraries(lb2 libgraphics.a SDL2main SDL2)
