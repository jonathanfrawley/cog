#ifndef COG_GROUP_H
#define COG_GROUP_H

#include "cog_list.h"
#include "cog_types.h"

typedef struct cog_group {
    cog_group_id id;
    cog_list anims;
} cog_group;

cog_group_id cog_group_add(void);
void cog_group_add_anim(cog_group_id id, cog_anim_id anim_id);
bool cog_group_collides(cog_group_id, cog_group_id);

//internal
cog_group* cog_group_get(cog_group_id id);
void cog_group_init(void);
void cog_group_removeall(void);

#endif // COG_GROUP_H
