cmake_minimum_required(VERSION 3.10.0)
project(DuckGame VERSION 0.1.0 LANGUAGES C CXX)

include_directories(include utils)
add_executable(DuckGame 
    src/main.cpp
    src/glad.c

    src/managers/GameManager.cpp

    src/states/PlayingState.cpp

    utils/glslprogram.cpp
    utils/GLUtils.cpp
    )

target_link_libraries(DuckGame glfw)

