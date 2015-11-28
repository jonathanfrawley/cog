cmake_minimum_required(VERSION 2.8)
#CLI Args
SET(BUILD_SDL false CACHE BOOL "Build with SDL or not")

project(cog C)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
find_package(SDL2 REQUIRED)
#find_package(PythonLibs REQUIRED)
find_package(PkgConfig REQUIRED)
#pkg_check_modules(JANSSON required jansson)
find_library(JANSSON_LIBRARY NAMES jansson)

#xml dep
find_package(LibXml2 REQUIRED)
INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})

if(BUILD_SDL)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    add_definitions(-DUSE_SDL)
    MESSAGE("Building using SDL2 for rendering and audio...")
    SET(COG_DEPS ${SDL2_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDL2MIXER_LIBRARY} ${SDL2TTF_LIBRARY})
    SET(cog_srcs cog_anim.c cog_core.c cog_graphics.c cog_graphics_sdl2.c cog_input.c cog_list.c cog_log.c cog_main.c cog_map.c cog_math.c cog_snd.c cog_snd_sdl2.c cog_sprite.c cog_state.c cog_test.c cog_text.c cog_window.c cog_text_sdl2.c cog_tween.c cog_group.c cog_rect.c)
else()
    MESSAGE("Building using SDL2 for windowing, OpenGL for rendering, OpenAL for audio...")
    find_package(OpenGL REQUIRED)
    INCLUDE(FindFreetype)
    IF(NOT FREETYPE_FOUND)
        FIND_LIBRARY(FREETYPE_LIBRARIES NAMES libfreetype freetype.dll PATHS "./libs/MacOS" "./libs/Windows" DOC "Freetype library")
        FIND_PATH(FREETYPE_INCLUDE_DIRS ftbuild.h "./includes" DOC "Freetype includes")
    ENDIF(NOT FREETYPE_FOUND)
    INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
    find_package(PNG REQUIRED)
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIRS})
    find_package(OpenGL REQUIRED)
    find_package(ALUT REQUIRED)
    find_package(OpenAL REQUIRED)
    SET(COG_DEPS ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${ALUT_LIBRARY} ${OPENAL_LIBRARY} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${LIBXML2_LIBRARIES} ${JANSSON_LIBRARY} GL)
    SET(cog_srcs cog_anim.c cog_core.c cog_graphics.c cog_graphics_opengl.c cog_input.c cog_list.c cog_log.c cog_main.c cog_map.c cog_math.c cog_snd_al.c cog_snd.c cog_sprite.c cog_state.c cog_test.c cog_text.c cog_text_freetype.c cog_window.c cog_window_sdl2.c cog_tween.c  cog_group.c cog_rect.c)
endif()

#Copy files to dest.
#file(COPY ../assets 
#    DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_BINARY_DIR})
file(COPY ../lang/swig/cog.i DESTINATION ${CMAKE_BINARY_DIR})
#add_custom_command(TARGET ALL PRE_BUILD
#    COMMAND swig -python cog.i)

#These are gcc specific, not sure they will generalise...
add_definitions(-std=c99 -O0 -Wall -g -DDEBUG -pg) #-pg is for profiling
set(CMAKE_BUILD_TYPE Debug)

add_library(cog ${cog_srcs})

if(PYTHONLIBS_FOUND)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    file(GLOB python_srcs ${CMAKE_BINARY_DIR}/*.c)
    add_library(_cog SHARED ${cog_srcs} ${python_srcs})
    target_link_libraries(_cog m ${COG_DEPS} ${PYTHON_LIBRARIES})
endif()

target_link_libraries(cog m ${COG_DEPS})

set(TEST_DIR ../tests)
set(EXAMPLE_DIR ../examples)
include_directories("${CMAKE_SOURCE_DIR}" "${TEST_DIR}")

add_definitions(-std=c99 -O0 -Wall -g -DDEBUG -pg) #-pg is for profiling
set(CMAKE_EXE_LINKER_FLAGS -pg)

add_executable(cog_test_list ${TEST_DIR}/test_list.c)
target_link_libraries(cog_test_list cog)
add_executable(cog_test_map ${TEST_DIR}/test_map.c)
target_link_libraries(cog_test_map cog)
add_executable(cog_test_state ${TEST_DIR}/test_state.c)
target_link_libraries(cog_test_state cog)

add_executable(cog_example_basic ${EXAMPLE_DIR}/example_basic.c)
target_link_libraries(cog_example_basic cog)
add_executable(cog_example_anim ${EXAMPLE_DIR}/example_anim.c)
target_link_libraries(cog_example_anim cog)
add_executable(cog_example_sound ${EXAMPLE_DIR}/example_sound.c)
target_link_libraries(cog_example_sound cog)
add_executable(cog_example_text ${EXAMPLE_DIR}/example_text.c)
target_link_libraries(cog_example_text cog)
add_executable(cog_example_sprite ${EXAMPLE_DIR}/example_sprite.c)
target_link_libraries(cog_example_sprite cog)
add_executable(cog_example_input ${EXAMPLE_DIR}/example_input.c)
target_link_libraries(cog_example_input cog)
add_executable(cog_example_grid ${EXAMPLE_DIR}/example_grid.c)
target_link_libraries(cog_example_grid cog)
add_executable(cog_example_tiled ${EXAMPLE_DIR}/example_tiled.c)
target_link_libraries(cog_example_tiled cog)
add_executable(cog_example_update ${EXAMPLE_DIR}/example_update.c)
target_link_libraries(cog_example_update cog)
add_executable(cog_example_gles ${EXAMPLE_DIR}/example_gles.c)
target_link_libraries(cog_example_gles cog)
add_executable(cog_example_shape ${EXAMPLE_DIR}/example_shape.c)
target_link_libraries(cog_example_shape cog)

# Installation
FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${headers} DESTINATION include)
#FILE(GLOB bins "${CMAKE_CURRENT_BINARY_DIR}/cog*")
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/cog_test DESTINATION bin)
