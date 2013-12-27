#include "cog_snd_al.h"

#include <AL/alut.h>

#include "cog_core.h"
#include "cog_map.h"

typedef struct {
    cog_snd_id id;
    ALuint buffer;
    ALuint source;
} cog_snd_al;

static cog_map snds;

void cog_snd_al_init() {
    int argc = 1;
    char* argv = "cog";
    alutInit(&argc, &argv);
    cog_map_init(&snds);
}

void cog_snd_al_add(const char* fname, cog_snd_id id) {
    cog_snd_al* snd = COG_STRUCT_MALLOC(cog_snd_al);
    snd->id = id;
    snd->buffer = alutCreateBufferFromFile(fname);
    alGenSources(1, &snd->source);
    alSourcei(snd->source, AL_BUFFER, snd->buffer);
    cog_map_put(&snds, snd->id, (void*) snd);
}

void cog_snd_al_play(cog_snd_id id) {
    cog_snd_al* snd = (cog_snd_al*) cog_map_get(&snds, id);
    alSourcePlay(snd->source);
}

void cog_snd_al_stop(cog_snd_id id) {
    cog_snd_al* snd = (cog_snd_al*) cog_map_get(&snds, id);
    alSourceStop(snd->source);
}
