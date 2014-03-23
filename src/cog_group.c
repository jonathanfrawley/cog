#include "cog_group.h"

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_map.h"

static cog_list active_groups;
static cog_map groups;
static cog_group_id groupcnt;

cog_group_id cog_group_add(void) {
    cog_group* group = COG_STRUCT_MALLOC(cog_group);
    group->id = groupcnt++;
    cog_list_init(& (group->anims), sizeof(cog_anim_id));
    cog_list_append(&active_groups, (cog_dataptr) & (group->id));
    cog_map_put(&groups, group->id, (void*) group);
    return group->id;
}

void cog_group_add_anim(cog_group_id id, cog_anim_id anim_id) {
    cog_group* group = cog_group_get(id);
    cog_list_append(&group->anims, (cog_dataptr) & (anim_id));
}

bool cog_group_collides(cog_group_id id0, cog_group_id id1) {
    cog_group* group0 = cog_group_get(id0);
    cog_group* group1 = cog_group_get(id1);
    COG_LIST_FOREACH(&group0->anims) {
        COG_LIST_FOREACH2(&group1->anims) {
            if(cog_anim_collides_anim(*(cog_anim_id*)curr->data, *(cog_anim_id*)curr2->data)) {
                return true;
            }
        }
    }
    return false;
}

cog_group* cog_group_get(cog_group_id id) {
    return (cog_group*) cog_map_get(&groups, id);
}

// Internal
void cog_group_init(void) {
    cog_map_init(&groups);
    cog_list_init(&active_groups, sizeof(cog_group_id));
}

void cog_group_removeall(void) {
    cog_list_removeall(&active_groups);
}
