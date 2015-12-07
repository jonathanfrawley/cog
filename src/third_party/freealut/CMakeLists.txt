
add_definitions(-DHAVE_CONFIG_H=1)

IF (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ###
    # Dependencies
    ###
    find_package(OpenAL REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
ENDIF (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")


# Without __NO_CTYPE tolower and friends are macros which introduce a GLIBC 2.3
# dependency. By defining this identifier we are currently backwards compatible
# to GLIBC 2.1.3, which is a good thing. In addition, the macros lead to code
# which triggers warnings with -Wunreachable-code.
if(HAVE_GLIBC)
	add_definitions(-D__NO_CTYPE=1)
endif()


if(HAVE_GCC_VISIBILITY)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	add_definitions(-DHAVE_GCC_VISIBILITY)
endif()

# So our static library can be linked into shared libraries (for instance,
# gcc on amd64 needs -fPIC)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")

set(ALUT_SOURCES
	alutBufferData.c
	alutCodec.c
	alutError.c
	alutInit.c
	alutInputStream.c
	alutLoader.c
	alutOutputStream.c
	alutUtil.c
	alutVersion.c
	alutWaveform.c)

set(ALUT_INTERNAL_HEADERS
	alutInternal.h)
source_group(InternalHeader FILES ${ALUT_INTERNAL_HEADERS})

set(ALUT_HEADERS
	AL/alut.h)
source_group(APIHeader FILES ${ALUT_HEADERS})

add_library(alut_static STATIC ${ALUT_SOURCES} ${ALUT_INTERNAL_HEADERS} ${ALUT_HEADERS})

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Cross compiling for windows, it is bloody hard to get OpenAL working, so just use statically compiled version from : http://wiki.flightgear.org/MinGW/cross-compiler
    target_link_libraries(alut_static ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/win32/libopenal32.a ${ADD_LIBS})
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(alut_static ${OPENAL_LIBRARY} ${ADD_LIBS})
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
if(UNIX)
    target_link_libraries(alut_static m)
endif()	

###
# Checking for types
###
if(WIN32)
	include(CheckTypeSize)
	check_type_size(__int8 HAVE___INT8)
endif()

###
# Checking for Includes
###
include(CheckIncludeFile)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(windows.h HAVE_WINDOWS_H)
check_include_file(features.h HAVE_FEATURES_H)

###
# Checking for functions
###
include(CheckSymbolExists)

check_symbol_exists(_stat "sys/types.h;sys/stat.h" HAVE__STAT)
if(HAVE_TIME_H)
	check_symbol_exists(nanosleep "time.h" HAVE_NANOSLEEP)
endif()
if(HAVE_UNISTD_H)
	check_symbol_exists(usleep "unistd.h" HAVE_USLEEP)
	check_symbol_exists(stat "sys/types.h;sys/stat.h;unistd.h" HAVE_STAT)
endif()
if(HAVE_WINDOWS_H)
	check_symbol_exists(Sleep "windows.h" HAVE_SLEEP)
endif()

###
# Checking for GLIBC
###
if(HAVE_FEATURES_H)
	check_symbol_exists(__GLIBC__ "features.h" HAVE_GLIBC)
endif()

###
# Checking for __attribute__
###
include(CheckCSourceCompiles)
check_c_source_compiles("void  foo (int bar __attribute__((unused)) ) { }
	static void baz (void) __attribute__((unused));
	static void baz (void) { }
	int main(){}
	"
	HAVE___ATTRIBUTE__)

###
# Checking for __attribute__((visibility("default"))) and -fvisibility=hidden
###
set(CMAKE_REQUIRED_FLAGS "-fvisibility=hidden")
check_c_source_compiles("void __attribute__((visibility(\"default\"))) test() {}
	#ifdef __INTEL_COMPILER
	#error ICC breaks with binutils and visibility
	#endif
	int main(){}
	" HAVE_GCC_VISIBILITY)
set(CMAKE_REQUIRED_FLAGS)


###
# Checking compiler flags
###
include(CheckCCompilerFlag)
set(NEW_FLAGS)
set(POTENTIAL_FLAGS)
if(NOT MSVC)
	if(BUILD_OPTIMIZATION)
		list(APPEND POTENTIAL_FLAGS -finline-functions -ffast-math)
	endif()

	if(BUILD_PROFILE)
		list(APPEND POTENTIAL_FLAGS -pg)
	elseif(BUILD_OPTIMIZATION)
		# -pg and -fomit-frame-pointer are incompatible
		list(APPEND POTENTIAL_FLAGS -fomit-frame-pointer)
	endif()

	if(BUILD_WARNINGS)
		list(APPEND POTENTIAL_FLAGS -Wall -ansi -pedantic -W)

		if(BUILD_MORE_WARNINGS)
			list(APPEND POTENTIAL_FLAGS -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual -Wdisabled-optimization -Wendif-labels -Winline -Wlong-long -Wmissing-declarations -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-compare -Wstrict-prototypes -Wwrite-strings)
		endif()

		if(BUILD_USE_WERROR)
			list(APPEND POTENTIAL_FLAGS -Werror)
		endif()
	else()

		if(BUILD_MORE_WARNINGS)
			message(STATUS "NOTE: BUILD_MORE_WARNINGS ignored because BUILD_WARNINGS not set!")
		endif()

		if(BUILD_USE_WERROR)
			message(STATUS "NOTE: BUILD_USE_WERROR ignored because BUILD_WARNINGS not set!")
		endif()
	endif()
endif()

if(POTENTIAL_FLAGS)
	foreach(FLAG ${POTENTIAL_FLAGS})
		string(REGEX REPLACE "^-" "COMPILER_SUPPORTS_" VAR "${FLAG}")
		check_c_compiler_flag("${FLAG}" ${VAR})
		if(${${VAR}})
			list(APPEND NEW_FLAGS ${FLAG})
		endif()
	endforeach()

	if(NEW_FLAGS)
		string(REPLACE ";" " " NEW_FLAGS "${NEW_FLAGS}")
		message(STATUS "Building with additional flags: ${NEW_FLAGS}")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${NEW_FLAGS}")
	endif()
endif()


###
# Generate the config header
###
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/AL/config.h.cmake_in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
