cmake_minimum_required(VERSION 3.28)
project(minesweeper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

if (WIN32)
    set(SDL2_PATH "C:/SDL2/x64")
endif (WIN32)

if (UNIX)
    # TODO: set Linux path
    set(SDL2_PATH "")
endif (UNIX)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

add_executable(
        minesweeper
        src/main.cpp
        src/engine.hpp
        src/core/game.hpp
        src/graphics/fonts.cpp
        src/graphics/textures.cpp
        src/screens/screen.hpp
        src/screens/main_menu_screen.hpp
        src/screens/game_screen.hpp
        app.rc
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})

add_custom_target(assets_data
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

add_dependencies(minesweeper assets_data)
