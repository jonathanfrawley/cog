#include "cog_snd.h"

#include "cog_core.h"
#include "cog_list.h"
#include "cog_map.h"

static cog_list activesnds;     //snds playing at the moment
static cog_snd_id cog_sndcnt;
static cog_map snds;

//sound

void cog_snd_init() {
    int argc = 1;
    char *argv = "cog";
    alutInit(&argc, &argv);

    cog_map_init(&snds);
    cog_list_init(&activesnds, sizeof(cog_sprite_id));
}
cog_snd_id cog_snd_load(char *fname) {
    cog_snd *snd = COG_STRUCT_MALLOC(cog_snd);
    snd->id = cog_sndcnt++;
    snd->buffer = alutCreateBufferFromFile(fname);
    alGenSources(1, &snd->source);
    alSourcei(snd->source, AL_BUFFER, snd->buffer);
    cog_map_put(&snds, snd->id, (void *) snd);
    return snd->id;
}

//TODO:Remove channel arg.
void cog_snd_play(cog_snd_id id) {
    cog_snd *snd = (cog_snd *) cog_map_get(&snds, id);
    alSourcePlay(snd->source);
    cog_list_append(&activesnds, (cog_dataptr) snd);
}

void cog_snd_play_sfx(cog_snd_id id) {
    cog_snd_play(id);
}

void cog_snd_play_music(cog_snd_id id) {
    cog_snd *snd = (cog_snd *) cog_map_get(&snds, id);
    alSourcei(snd->source, AL_LOOPING, AL_TRUE);
    cog_snd_play(id);
}

void cog_snd_stop(cog_snd_id id) {
    cog_snd *snd = (cog_snd *) cog_map_get(&snds, id);
    alSourceStop(snd->source);
    cog_list_remove(&activesnds, (cog_dataptr) snd);
}

void cog_snd_stopall() {
    COG_LIST_FOREACH(&activesnds) {
        cog_snd *snd = (cog_snd *) curr->data;
        cog_snd_stop(snd->id);
    }
}
