#ifndef  COG_SND_H
#define  COG_SND_H

#include "cog_types.h"

void cog_snd_init(void);
cog_snd_id cog_snd_add(const char* fname);
cog_snd_id cog_snd_add_mus(const char* fname);
void cog_snd_play(cog_snd_id id);
void cog_snd_stop(cog_snd_id id);
void cog_snd_stopall();
void cog_snd_quit();

#endif // COG_SND_H
