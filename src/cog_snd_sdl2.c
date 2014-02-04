#include "cog_snd_sdl2.h"

#include <SDL2/SDL_mixer.h>

#include "cog_core.h"
#include "cog_log.h"
#include "cog_map.h"

typedef enum {
    SOUND,
    MUSIC
} cog_snd_sdl2_type;

typedef struct {
    cog_snd_id id;
    cog_snd_sdl2_type type;
    Mix_Music* mus;
    Mix_Chunk* snd;
    int32_t channel;
} cog_snd_sdl2;

static cog_map snds;

void cog_snd_sdl2_init(void) {
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cog_errorf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
    cog_map_init(&snds);
}

void cog_snd_sdl2_add(const char* fname, cog_snd_id id) {
    cog_snd_sdl2* snd = COG_STRUCT_MALLOC(cog_snd_sdl2);
    snd->snd = Mix_LoadWAV(fname);
    if(snd->snd==0) {
        cog_errorf("Could not load sound : %s\n", Mix_GetError());
    }
    snd->id = id;
    snd->type = SOUND;
    cog_map_put(&snds, id, (void*) snd);
}

void cog_snd_sdl2_add_mus(const char* fname, cog_snd_id id) {
    cog_snd_sdl2* snd = COG_STRUCT_MALLOC(cog_snd_sdl2);
    snd->mus = Mix_LoadMUS(fname);
    if(snd->mus==0) {
        cog_errorf("Could not load music : %s\n", Mix_GetError());
    }
    snd->id = id;
    snd->type = MUSIC;
    cog_map_put(&snds, id, (void*) snd);
}

void cog_snd_sdl2_play(cog_snd_id id) {
    cog_snd_sdl2* snd = (cog_snd_sdl2*) cog_map_get(&snds, id);
    if(snd->type == SOUND) {
        snd->channel = Mix_PlayChannel(-1, snd->snd, 0);
    } else {
        Mix_PlayMusic(snd->mus, -1);
    }
}

void cog_snd_sdl2_stop(cog_snd_id id) {
    cog_snd_sdl2* snd = (cog_snd_sdl2*) cog_map_get(&snds, id);
    if(snd->type == SOUND) {
        Mix_HaltChannel(snd->channel);
    } else {
        Mix_HaltMusic();
    }
}

void cog_snd_sdl2_quit() {
}
