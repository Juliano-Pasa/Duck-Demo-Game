cmake_minimum_required(VERSION 3.10.0)
project(DuckGame VERSION 0.1.0 LANGUAGES C CXX)

include_directories(include utils)
add_executable(DuckGame 
    src/main.cpp
    src/glad.c
    src/lodepng.cpp

    src/controllers/DuckBotController.cpp
    src/controllers/DuckPlayerController.cpp
    src/controllers/EntityController.cpp

    src/entities/Camera.cpp
    src/entities/Duck.cpp
    src/entities/Entity.cpp
    src/entities/Transform.cpp
    src/entities/Wind.cpp
    src/entities/WorldLight.cpp

    src/managers/CollisionManager.cpp
    src/managers/GameManager.cpp
    src/managers/InputManager.cpp

    src/states/PlayingState.cpp

    src/terrain/TerrainGenerator.cpp
    src/terrain/TerrainV2.cpp

    utils/glslprogram.cpp
    utils/GLUtils.cpp
    )

target_link_libraries(DuckGame glfw)

