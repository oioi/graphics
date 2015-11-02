set(SOURCES main.cpp)
add_executable(lb3 ${SOURCES})
target_link_libraries(lb3 libgraphics.a SDL2main SDL2 SDL2_ttf)
