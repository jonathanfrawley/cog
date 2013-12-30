#include "cog_snd.h"

#ifdef USE_SDL
#include <cog_snd_sdl2.h>
#else
#include <cog_snd_al.h>
#endif
#include "cog_core.h"
#include "cog_list.h"
#include "cog_map.h"

typedef struct {
    void (*snd_init)(void);
    void (*snd_add)(const char* fname, cog_snd_id id);
    void (*snd_add_mus)(const char* fname, cog_snd_id id);
    void (*snd_play)(cog_snd_id id);
    void (*snd_stop)(cog_snd_id id);
} cog_snd_player;

static cog_snd_player player;
static cog_list activesnds;     //snds playing at the moment
static cog_snd_id cog_snd_cnt;
//sound

void cog_snd_init() {
#ifdef USE_SDL
    player.snd_init = cog_snd_sdl2_init;
    player.snd_add = cog_snd_sdl2_add;
    player.snd_add_mus = cog_snd_sdl2_add_mus;
    player.snd_play = cog_snd_sdl2_play;
    player.snd_stop = cog_snd_sdl2_stop;
#else
    player.snd_init = cog_snd_al_init;
    player.snd_add = cog_snd_al_add;
    player.snd_add_mus = cog_snd_al_add;
    player.snd_play = cog_snd_al_play;
    player.snd_stop = cog_snd_al_stop;
#endif
    cog_list_init(&activesnds, sizeof(cog_snd_id));
    player.snd_init();
}

cog_snd_id cog_snd_add(const char* fname) {
    cog_snd_id id = cog_snd_cnt++;
    player.snd_add(fname, id);
    return id;
}

cog_snd_id cog_snd_add_mus(const char* fname) {
    cog_snd_id id = cog_snd_cnt++;
    player.snd_add_mus(fname, id);
    return id;
}

void cog_snd_play(cog_snd_id id) {
    player.snd_play(id);
    cog_list_append(&activesnds, (cog_dataptr) &id);
}

void cog_snd_stop(cog_snd_id id) {
    player.snd_stop(id);
    cog_list_remove(&activesnds, (cog_dataptr) &id);
}

void cog_snd_stopall() {
    COG_LIST_FOREACH(&activesnds) {
        cog_snd_id* snd = (cog_snd_id*) curr->data;
        cog_snd_stop(*snd);
    }
}
