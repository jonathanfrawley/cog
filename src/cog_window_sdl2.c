#include "cog_window_sdl2.h"

#include <SDL2/SDL.h>
#include "cog_log.h"
#include "cog_window.h"

SDL_Window* sdl_window;
SDL_GLContext glcontext;
SDL_Surface* screen_surface;

void cog_window_sdl2_init(cog_config config, cog_window* window) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cog_errorf(SDL_GetError());
    }
    int width = config.window_w;
    int height = config.window_h;
    if(config.window_w == 0) {
        width = 640;
        height = 600;
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
    }
    if((sdl_window = SDL_CreateWindow("cog game",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width, height,
                                      flags)) == 0) {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n",
                   SDL_GetError());
    }
#ifndef USE_SDL
    glcontext = SDL_GL_CreateContext(sdl_window);
    if(SDL_GL_SetSwapInterval(1) < 0) {
        cog_debugf("cog_window_init: Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }
#endif
    window->w = (double)width;
    window->h = (double)height;
    screen_surface = SDL_GetWindowSurface(sdl_window);
}

void cog_window_sdl2_update(cog_window* window) {
}

void cog_window_sdl2_quit(cog_window* window) {
#ifndef USE_SDL
    SDL_GL_DeleteContext(glcontext);
#endif
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

void cog_window_sdl2_toggle_fullscreen(cog_window* window) {
    SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

SDL_Window* cog_window_sdl2_get_window(void) {
    return sdl_window;
}

