set(SOURCES main.cpp)
add_executable(lb1 ${SOURCES})
target_link_libraries(lb1 libgraphics.a SDL2main SDL2 SDL2_ttf)
