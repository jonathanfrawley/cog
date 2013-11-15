#ifndef  COG_SND_H
#define  COG_SND_H

#include <AL/alut.h>

#include "cog_types.h"

typedef struct {
    cog_snd_id id;
    ALuint buffer;
    ALuint source;
} cog_snd;

void cog_snd_init(void);
cog_snd_id cog_snd_add(const char* fname);
void cog_snd_play(cog_snd_id id);
void cog_snd_play_sfx(cog_snd_id snd);
void cog_snd_play_music(cog_snd_id snd);
void cog_snd_stop(cog_snd_id id);
void cog_snd_stopall();

#endif // COG_SND_H
