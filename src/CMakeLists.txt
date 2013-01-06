cmake_minimum_required(VERSION 2.8)
project(cog C)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
# GLES
#find_package(GLEW REQUIRED)
#find_package(OpenGL REQUIRED)
find_package(OpenGLES REQUIRED)
find_package(EGL REQUIRED)

find_package(ALUT REQUIRED)
find_package(OpenAL REQUIRED)
add_definitions(-std=c99 -Werror -Wall)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB srcs *.c)
add_library(cog ${srcs})
# GLES
#target_link_libraries(cog ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_LIBRARY} m ${ALUT_LIBRARY} ${OPENAL_LIBRARY})
#target_link_libraries(cog m ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${ALUT_LIBRARY} ${OPENAL_LIBRARY})
target_link_libraries(cog m ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${ALUT_LIBRARY} ${OPENAL_LIBRARY} ${OPENGLES_LIBRARIES} ${EGL_LIBRARIES})

#GLES (#ifdef out)
add_definitions(-DWITH_GLES)

set(TEST_DIR ../tests)
include_directories("${CMAKE_SOURCE_DIR}" "${TEST_DIR}")

add_executable(cog_test ${TEST_DIR}/test.c)
target_link_libraries(cog_test cog)
add_executable(cog_test_anim ${TEST_DIR}/test_anim.c)
target_link_libraries(cog_test_anim cog)
add_executable(cog_test_sound ${TEST_DIR}/test_sound.c)
target_link_libraries(cog_test_sound cog)
add_executable(cog_test_text ${TEST_DIR}/test_text.c)
target_link_libraries(cog_test_text cog)
add_executable(cog_test_list ${TEST_DIR}/test_list.c)
target_link_libraries(cog_test_list cog)
add_executable(cog_test_state ${TEST_DIR}/test_state.c)
target_link_libraries(cog_test_state cog)

# Installation
FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${headers} DESTINATION include)
#FILE(GLOB bins "${CMAKE_CURRENT_BINARY_DIR}/cog*")
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/cog_test DESTINATION bin)
