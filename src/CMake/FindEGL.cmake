
# - Try to find OpenGLES
# Once done this will define
#  
#  EGL_FOUND        - system has OpenGLES
#  EGL_INCLUDE_DIR  - the GL include directory
#  EGL_LIBRARIES    - Link these to use OpenGLES

IF (WIN32)
  IF (CYGWIN)

    FIND_PATH(EGL_INCLUDE_DIR GLES/gl.h )

    FIND_LIBRARY(EGL_gl_LIBRARY libgles_cm )

  ELSE (CYGWIN)

    IF(BORLAND)
      SET (EGL_gl_LIBRARY import32 CACHE STRING "OpenGL library for win32")
    ELSE(BORLAND)
	  #MS compiler - todo - fix the following line:
      SET (EGL_gl_LIBRARY ${OGRE_SOURCE_DIR}/Dependencies/lib/release/libgles_cm.lib CACHE STRING "OpenGL library for win32")
    ENDIF(BORLAND)

  ENDIF (CYGWIN)

ELSE (WIN32)

  IF (APPLE)

	create_search_paths(/Developer/Platforms)
	findpkg_framework(OpenGLES)
    set(EGL_gl_LIBRARY "-framework OpenGLES")

  ELSE(APPLE)



    FIND_PATH(EGL_INCLUDE_DIR EGL/egl.h
      /usr/openwin/share/include
      /opt/graphics/OpenGL/include /usr/X11R6/include
      /usr/include
    )


    FIND_LIBRARY(EGL_gl_LIBRARY
      NAMES EGL
      PATHS /opt/graphics/OpenGL/lib
            /usr/openwin/lib
            /usr/shlib /usr/X11R6/lib
            /usr/lib
    )

    # On Unix OpenGL most certainly always requires X11.
    # Feel free to tighten up these conditions if you don't 
    # think this is always true.
    # It's not true on OSX.

    IF (EGL_gl_LIBRARY)
      IF(NOT X11_FOUND)
        INCLUDE(FindX11)
      ENDIF(NOT X11_FOUND)
      IF (X11_FOUND)
        IF (NOT APPLE)
          SET (EGL_LIBRARIES ${X11_LIBRARIES})
        ENDIF (NOT APPLE)
      ENDIF (X11_FOUND)
    ENDIF (EGL_gl_LIBRARY)

  ENDIF(APPLE)
ENDIF (WIN32)

SET( EGL_FOUND "NO" )
IF(EGL_gl_LIBRARY)

    SET( EGL_LIBRARIES ${EGL_gl_LIBRARY} ${EGL_LIBRARIES})

    SET( EGL_FOUND "YES" )

ENDIF(EGL_gl_LIBRARY)

MARK_AS_ADVANCED(
  EGL_INCLUDE_DIR
  EGL_gl_LIBRARY
)
