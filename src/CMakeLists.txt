CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(cog C)

SET(CMAKE_BUILD_TYPE Debug)
#ADD_DEFINITIONS(-std=c99 -O0 -Wall -g -DDEBUG) #-pg is for profiling
#ADD_DEFINITIONS(-std=c99 -static-libgcc) #-pg is for profiling
#SET(CMAKE_EXE_LINKER_FLAGS -pg)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

# Finding dependencies 
MESSAGE("Building using SDL2 for windowing, OpenGL for rendering, OpenAL for audio...")
FIND_PACKAGE(SDL2 REQUIRED)
#FIND_PACKAGE(PythonLibs REQUIRED)  # TODO: For python support below
FIND_PACKAGE(PkgConfig REQUIRED)
FIND_LIBRARY(JANSSON_LIBRARY NAMES jansson)
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE(FindFreetype)
IF(NOT FREETYPE_FOUND)
    FIND_LIBRARY(FREETYPE_LIBRARIES NAMES libfreetype freetype.dll PATHS "./libs/MacOS" "./libs/Windows" DOC "Freetype library")
    FIND_PATH(FREETYPE_INCLUDE_DIRS ftbuild.h "./includes" DOC "Freetype includes")
ENDIF(NOT FREETYPE_FOUND)
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
FIND_PACKAGE(PNG REQUIRED)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIRS})
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(ALUT REQUIRED)
IF(NOT ALUT_FOUND)
    MESSAGE("Could not find ALUT")
ENDIF(NOT ALUT_FOUND)
FIND_PACKAGE(OpenAL REQUIRED)

MESSAGE("ALUT_LIBRARY: " + ${ALUT_LIBRARY})
MESSAGE("OPENAL_LIBRARY: " + ${OPENAL_LIBRARY})
MESSAGE("SDL2_LIBRARY: " + ${SDL2_LIBRARY})

# Setting dependencies list
SET(COG_DEPS ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${ALUT_LIBRARY} ${OPENAL_LIBRARY} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${JANSSON_LIBRARY})
# Windows needs some extra libraries
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(COG_DEPS ${COG_DEPS} -limm32 -lversion )
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# List of source files in library
SET(cog_srcs cog_anim.c cog_core.c cog_graphics.c cog_graphics_opengl.c cog_input.c cog_list.c cog_log.c cog_main.c cog_map.c cog_math.c cog_snd.c cog_sprite.c cog_state.c cog_test.c cog_text.c cog_text_freetype.c cog_window.c cog_window_sdl2.c cog_tween.c  cog_group.c cog_rect.c cog_snd_al.c)

ADD_LIBRARY(cog ${cog_srcs})

# TODO: Look into python support again
#IF(PYTHONLIBS_FOUND)
    #INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    #FILE(GLOB python_srcs ${CMAKE_BINARY_DIR}/*.c)
    #ADD_LIBRARY(_cog SHARED ${cog_srcs} ${python_srcs})
    #TARGET_LINK_LIBRARIES(_cog m ${COG_DEPS} ${PYTHON_LIBRARIES})
#ENDIF()

TARGET_LINK_LIBRARIES(cog m ${COG_DEPS})

SET(TEST_DIR ../tests)
SET(EXAMPLE_DIR ../examples)
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}" "${TEST_DIR}")

# Do "make tests" to build examples
ADD_CUSTOM_TARGET(tests)
ADD_EXECUTABLE(cog_test_list EXCLUDE_FROM_ALL ${TEST_DIR}/test_list.c)
TARGET_LINK_LIBRARIES(cog_test_list cog)
ADD_EXECUTABLE(cog_test_map EXCLUDE_FROM_ALL ${TEST_DIR}/test_map.c)
TARGET_LINK_LIBRARIES(cog_test_map cog)
ADD_EXECUTABLE(cog_test_state EXCLUDE_FROM_ALL ${TEST_DIR}/test_state.c)
TARGET_LINK_LIBRARIES(cog_test_state cog)
ADD_DEPENDENCIES(tests cog_test_list cog_test_map cog_test_state)

# Do "make examples" to build examples
ADD_CUSTOM_TARGET(examples)
ADD_EXECUTABLE(cog_example_basic EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_basic.c)
TARGET_LINK_LIBRARIES(cog_example_basic cog)
ADD_EXECUTABLE(cog_example_anim EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_anim.c)
TARGET_LINK_LIBRARIES(cog_example_anim cog)
ADD_EXECUTABLE(cog_example_sound EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_sound.c)
TARGET_LINK_LIBRARIES(cog_example_sound cog)
ADD_EXECUTABLE(cog_example_text EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_text.c)
TARGET_LINK_LIBRARIES(cog_example_text cog)
ADD_EXECUTABLE(cog_example_sprite EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_sprite.c)
TARGET_LINK_LIBRARIES(cog_example_sprite cog)
ADD_EXECUTABLE(cog_example_input EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_input.c)
TARGET_LINK_LIBRARIES(cog_example_input cog)
ADD_EXECUTABLE(cog_example_grid EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_grid.c)
TARGET_LINK_LIBRARIES(cog_example_grid cog)
ADD_EXECUTABLE(cog_example_tiled EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_tiled.c)
TARGET_LINK_LIBRARIES(cog_example_tiled cog)
ADD_EXECUTABLE(cog_example_update EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_update.c)
TARGET_LINK_LIBRARIES(cog_example_update cog)
ADD_EXECUTABLE(cog_example_gles EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_gles.c)
TARGET_LINK_LIBRARIES(cog_example_gles cog)
ADD_EXECUTABLE(cog_example_shape EXCLUDE_FROM_ALL ${EXAMPLE_DIR}/example_shape.c)
TARGET_LINK_LIBRARIES(cog_example_shape cog)
ADD_DEPENDENCIES(examples cog_example_basic cog_example_anim cog_example_sound cog_example_text cog_example_sprite cog_example_input cog_example_grid cog_example_tiled cog_example_update cog_example_gles cog_example_shape)

# Installation
FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${headers} DESTINATION include)
#FILE(GLOB bins "${CMAKE_CURRENT_BINARY_DIR}/cog*")
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/cog_test DESTINATION bin)
