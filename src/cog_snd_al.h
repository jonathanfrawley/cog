#ifndef  COG_SND_AL_H
#define  COG_SND_AL_H

#include "cog_types.h"

void cog_snd_al_init(void);
void cog_snd_al_add(const char* fname, cog_snd_id id);
void cog_snd_al_play(cog_snd_id id);
void cog_snd_al_stop(cog_snd_id id);

#endif
