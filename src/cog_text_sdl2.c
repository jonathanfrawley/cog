#include "cog_text_sdl2.h"

#include "cog_log.h"
#include "cog_map.h"
#include "cog_types.h"

static cog_map text_sdl2s;

static TTF_Font* default_face;
static const cog_string default_path = "media/font/04B_03__.ttf";
static uint32_t default_pt_size = 48;

void cog_text_sdl2_add(cog_text_id id) {
    cog_text_sdl2* text_sdl2 = COG_STRUCT_MALLOC(cog_text_sdl2);
    text_sdl2->id = id;
    text_sdl2->face = default_face;
    cog_map_put(&text_sdl2s, id, (cog_dataptr) text_sdl2);
}

void cog_text_sdl2_set_face(cog_text_id id, cog_string path, double pt_size) {
    cog_text_sdl2* text_sdl2 = cog_map_get(&text_sdl2s, id);
    text_sdl2->face = TTF_OpenFont(path, pt_size);
}

void cog_text_sdl2_remove(cog_text_id id) {
    cog_map_remove(&text_sdl2s, id);
}

void cog_text_sdl2_init(void) {
    //Initialize SDL_ttf
    if(TTF_Init() == -1) {
        cog_errorf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }
    default_face = TTF_OpenFont(default_path, default_pt_size);
    cog_map_init(&text_sdl2s);
}

cog_text_sdl2* cog_text_sdl2_get(cog_text_id id) {
    return (cog_text_sdl2*) cog_map_get(&text_sdl2s, id);
}
