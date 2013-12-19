#include "cog_graphics_sdl2.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cog_main.h>
#include <cog_map.h>
#include <cog_window.h>

static cog_map textures;
static uint32_t texture_cnt;
static SDL_Renderer* renderer;
static cog_window* window;

void cog_graphics_sdl2_draw_sprite(cog_sprite* sprite) {
    SDL_Texture* texture = (SDL_Texture*)cog_map_get(&textures, sprite->tex_id);
    //TODO: Define this properly
    SDL_Rect texr;
    texr.x = 0;
    texr.y = 0;
    texr.w = 400;
    texr.h = 300;
    SDL_RenderCopy(renderer, texture, NULL, &texr);
}

void cog_graphics_sdl2_init(cog_window* win) {
    cog_map_init(&textures);
    window = win;
    renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);
}

void cog_graphics_sdl2_draw_text(cog_text* text) {
    //TODO
}

uint32_t cog_graphics_sdl2_gen_tex_id() {
    return 0; //Not sure...
}

uint32_t cog_graphics_sdl2_load_texture(const char* filename, int* width, int* height) {
    SDL_Texture* img = IMG_LoadTexture(renderer, filename);
    int id = texture_cnt;
    cog_map_put(&textures, id, (cog_dataptr)img);
    texture_cnt++;
    return id;
}

void cog_graphics_sdl2_clear(void) {
    SDL_RenderClear(renderer);
}

void cog_graphics_sdl2_flush(void) {
    SDL_RenderPresent(renderer);
}
