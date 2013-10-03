#include "cog_window.h"

#include "cog_log.h"

void cog_window_init(cog_window* window) {
    //if( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        cog_errorf(SDL_GetError());
    }
#if defined(HAVE_GLES)
    if(EGL_Open()) {
        cog_errorf("Failed initialising EGL");
    }
#endif
    //TODO:Get from yaml conf.
#if !defined(HAVE_GLES)
    int width = 800;
    int height = 600;
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#else
    /*
    int width = 640;
    int height = 480;
    int flags = SDL_SWSURFACE | SDL_FULLSCREEN;
    */
#endif
    //Use OpenGL 2.1
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

    if((window->screen = SDL_CreateWindow("cog game",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          width, height,
                                          flags)) == 0) {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n",
                   SDL_GetError());
    }
    window->glcontext = SDL_GL_CreateContext(window->screen);
#if defined(HAVE_GLES)
    EGL_Init();
#endif
    if( SDL_GL_SetSwapInterval(1) < 0 ) {
        cog_debugf("cog_window_init: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }
}

void cog_window_update(cog_window* window) {
    SDL_GL_SwapWindow(window->screen);
}

void cog_window_quit(cog_window* window) {
    //SDL_GL_DeleteContext(window->glcontext);
    SDL_DestroyWindow(window->screen);
    SDL_Quit();
}

void cog_window_toggle_fullscreen(cog_window* window) {
    SDL_SetWindowFullscreen(window->screen, 0);
}
