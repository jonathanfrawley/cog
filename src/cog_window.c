#include "cog_window.h"

#include "cog_log.h"

void cog_window_init(cog_config config, cog_window* window) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cog_errorf(SDL_GetError());
    }
    int width = config.window_w;
    int height = config.window_h;
    if(config.window_w == 0) {
        width = 640;
        height = 480;
    }
#ifdef USE_SDL
    int flags = SDL_RENDERER_ACCELERATED;
#else
    int flags = SDL_WINDOW_OPENGL;
    //Use OpenGL 2.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
    if(config.fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
        width = 0;
        height = 0;
    }
    if((window->window = SDL_CreateWindow("cog game",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          width, height,
                                          flags)) == 0) {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n",
                   SDL_GetError());
    }
#ifndef USE_SDL
    window->glcontext = SDL_GL_CreateContext(window->window);
    if(SDL_GL_SetSwapInterval(1) < 0) {
        cog_debugf("cog_window_init: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }
#endif
    window->w = (double)width;
    window->h = (double)height;
    window->screen_surface = SDL_GetWindowSurface(window->window);
}

void cog_window_update(cog_window* window) {
//    SDL_GL_SwapWindow(window->window);
}

void cog_window_quit(cog_window* window) {
#ifndef USE_SDL
    SDL_GL_DeleteContext(window->glcontext);
#endif
    SDL_DestroyWindow(window->window);
    SDL_Quit();
}

void cog_window_toggle_fullscreen(cog_window* window) {
    SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}
