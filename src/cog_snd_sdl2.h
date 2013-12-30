#ifndef  COG_SND_SDL_H
#define  COG_SND_SDL_H

#include "cog_types.h"

void cog_snd_sdl2_init(void);
void cog_snd_sdl2_add(const char* fname, cog_snd_id id);
void cog_snd_sdl2_add_mus(const char* fname, cog_snd_id id);
void cog_snd_sdl2_play(cog_snd_id id);
void cog_snd_sdl2_stop(cog_snd_id id);
void cog_snd_sdl2_shutdown();

#endif
