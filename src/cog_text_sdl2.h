#ifndef  COG_TEXT_SDL2_H
#define  COG_TEXT_SDL2_H

#include <SDL2/SDL_ttf.h>

#include "cog_core.h"
#include "cog_types.h"

void cog_text_sdl2_add(cog_text_id id);
void cog_text_sdl2_set_face(cog_text_id id, cog_string path, double pt_size);
void cog_text_sdl2_remove(cog_text_id id);
void cog_text_sdl2_init(void);

//Internal
typedef struct {
    cog_text_id id;
    TTF_Font* face;
    SDL_Surface* surface;
} cog_text_sdl2;

cog_text_sdl2* cog_text_sdl2_get(cog_text_id id);

#endif
