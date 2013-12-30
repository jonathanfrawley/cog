#include "cog_graphics_sdl2.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cog_log.h>
#include <cog_main.h>
#include <cog_map.h>
#include <cog_math.h>
#include <cog_window.h>

static cog_map textures;
static uint32_t texture_cnt;
static SDL_Renderer* renderer;
static cog_window* window;
static cog_dim2 win_dim;

/* Scales pos to use new dimension system passed in. */
cog_pos2 scale_pos2(cog_pos2 pos, cog_dim2 dim) {
    cog_pos2 res;
    res.x = pos.x * dim.w;
    res.y = pos.y * dim.h;
    return res;
}

cog_dim2 scale_dim2(cog_dim2 dim1, cog_dim2 dim2) {
    cog_dim2 res;
    res.w = dim1.w * dim2.w;
    res.h = dim1.h * dim2.h;
    return res;
}

void cog_graphics_sdl2_draw_sprite(cog_sprite* sprite) {
    // Input sprite coord system:
    //        *----------* (1,1)
    //        |    .(0,0)|
    // (-1,-1)*----------*
    //##################################
    // Output sprite coord system:
    //             (0,0)*----------* (SCREEN_WIDTH,0)
    //                  |    .(x,y)|
    // (0,SCREEN_HEIGHT)*----------* (SCREEN_WIDTH,SCREEN_HEIGHT)
    //##################################
    // Transform:
    //    1) Add one to pos on x and y (now it it from 0,0 to 2,2 with 0,0 being the bottom left)
    // (0,2)*----------* (2,2)
    //      |    .(1,1)|
    // (0,0)*----------* (2,0)
    //    2) Invert the y axis:
    // (0,0)*----------* (2,0)
    //      |    .(1,1)|
    // (0,2)*----------* (2,2)
    //    3) Scale up to desired res (multiply x by SCREEN_WIDTH, multiply y by SCREEN_HEIGHT):
    //               (0,0)*----------* (2*SCREEN_WIDTH,0)
    //                    |    .(x,y)|
    // (0,2*SCREEN_HEIGHT)*----------* (2*SCREEN_WIDTH,2*SCREEN_HEIGHT)
    //    4) Transform back (Subtract SCREEN_WIDTH from x axis and SCREEN_HEIGHT on y axis)
    //               (0,0)*----------* (SCREEN_WIDTH,0)
    //                    |    .(x,y)|
    // (0,SCREEN_HEIGHT)*----------* (SCREEN_WIDTH,SCREEN_HEIGHT)
    SDL_Texture* texture = (SDL_Texture*)cog_map_get(&textures, sprite->tex_id);
    cog_dim2 sdl2_dim_tmp = scale_dim2(sprite->dim, (cog_dim2) {
        .w=2.0f, .h=2.0f
    });
    cog_dim2 sdl2_dim = scale_dim2(sdl2_dim_tmp, win_dim);
    cog_pos2 sdl2_pos = sprite->pos;
    //2)
    sdl2_pos.y *= -1.0;
    //1)
    sdl2_pos.x += 1.0;
    sdl2_pos.y += 1.0;
    //3)
    sdl2_pos.x *= (win_dim.w * 0.5);
    sdl2_pos.y *= (win_dim.h * 0.5);
    //4)
    sdl2_pos.x -= (win_dim.w * 0.5);
    sdl2_pos.y -= (win_dim.h * 0.5);
    //5)?
    sdl2_pos.x += (win_dim.w * 0.5) - sdl2_dim.w*(0.25);
    sdl2_pos.y += (win_dim.h * 0.5) - sdl2_dim.h*(0.25);
    int tex_w, tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);
    cog_dim2 sdl2_tex_full_dim;
    sdl2_tex_full_dim.w = tex_w;
    sdl2_tex_full_dim.h = tex_h;
    cog_pos2 sdl2_tex_pos = scale_pos2(sprite->tex_pos, sdl2_tex_full_dim);
    cog_dim2 sdl2_tex_dim = scale_dim2(sprite->tex_dim, sdl2_tex_full_dim);
    SDL_Rect texsrc;
    texsrc.x = sdl2_tex_pos.x;
    texsrc.y = sdl2_tex_pos.y;
    texsrc.w = sdl2_tex_dim.w;
    texsrc.h = sdl2_tex_dim.h;
    SDL_Rect texr;
    texr.x = sdl2_pos.x;
    texr.y = sdl2_pos.y;
    texr.w = sdl2_dim.w*(0.5);
    texr.h = sdl2_dim.h*(0.5);
    SDL_RenderCopyEx(renderer, texture, &texsrc, &texr, cog_math_radians_to_degrees(-sprite->rot), NULL, SDL_FLIP_NONE);
    //Render red filled quad
    int WIDTH = 10;
    int HEIGHT = 10;
    int x = texr.x+(texr.w/2)-WIDTH/2;
    int y = texr.y+(texr.h/2)-HEIGHT/2;
    SDL_Rect fillRect = { x, y, WIDTH, HEIGHT};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_SetRenderDrawColor(renderer, 0x48, 0x00, 0x00, 0xFF);
}

void cog_graphics_sdl2_init(cog_window* win) {
    cog_map_init(&textures);
    window = win;
    int w, h;
    SDL_GetWindowSize(window->window,
                      &w,
                      &h);
    win_dim.w = (float)w;
    win_dim.h = (float)h;
    renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);
}

void cog_graphics_sdl2_draw_text(cog_text* text) {
    //TODO
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
