#ifndef COG_TWEEN_H
#define COG_TWEEN_H

#include "cog_types.h"

cog_tween_id cog_tween_pos_add(cog_pos2 pos, cog_pos2 dest_pos, double time);
cog_pos2 cog_tween_pos_get(cog_tween_id id);
bool cog_tween_finished(cog_tween_id id);

void cog_tween_init(void);
void cog_tween_update(double timestep);
void cog_tween_removeall(void);

#endif
