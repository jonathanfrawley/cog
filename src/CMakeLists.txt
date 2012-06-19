cmake_minimum_required(VERSION 2.8)
project(cog C)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
add_definitions(-std=c99)

set(CMAKE_BUILD_TYPE Debug)

add_library (cog cog.c cog_map.c cog_core.c cog_list.c )
target_link_libraries(cog ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_LIBRARY} m)

add_executable(test test.c)
target_link_libraries(test cog)
add_executable(testanim testanim.c)
target_link_libraries(testanim cog)
add_executable(testsound testsound.c)
target_link_libraries(testsound cog)

