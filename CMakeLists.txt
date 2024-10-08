cmake_minimum_required(VERSION 3.28)
project(minesweeper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

if (WIN32)
    set(SDL2_PATH "C:/SDL2/x64")
endif (WIN32)

if (UNIX)
    set(SDL2_PATH "/usr")
endif (UNIX)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} include/SDL2_gfx)

file(GLOB SDL2_gfx_source "include/SDL2_gfx/*.c" "include/SDL2_gfx/*.h")

add_executable(
        minesweeper
        ${SDL2_gfx_source}
        src/main.cpp
        src/engine.hpp
        src/core/game.hpp
        src/core/settings.hpp
        src/graphics/color.hpp
        src/graphics/font.hpp
        src/graphics/shape.hpp
        src/graphics/texture.hpp
        src/graphics/texture_bundle.hpp
        src/texture_managers/game_texture_manager.hpp
        src/texture_managers/main_menu_texture_manager.hpp
        src/texture_managers/settings_texture_manager.hpp
        src/screens/screen.hpp
        src/screens/main_menu_screen.hpp
        src/screens/game_screen.hpp
        src/screens/settings_screen.hpp
        app.rc
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY})

add_custom_target(assets_data
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

add_dependencies(minesweeper assets_data)
