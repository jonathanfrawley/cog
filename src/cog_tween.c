#include "cog_tween.h"

#include "cog_core.h"
#include "cog_list.h"
#include "cog_log.h"
#include "cog_map.h"
#include "cog_math.h"

static cog_tween_id tweencnt;
static cog_list active_tweens;
static cog_map tweens;

typedef enum {
    COG_T_POS
} cog_tween_type;

typedef struct cog_tween {
    cog_tween_id id;
    cog_tween_type type;
    cog_pos2 pos;
    cog_pos2 dest_pos;
    double time;
    double time_progress;
    double percent_progress;
    bool finished;
} cog_tween;

void cog_tween_init(void) {
    cog_map_init(&tweens);
    cog_list_init(&active_tweens, sizeof(cog_tween_id));
}

cog_tween_id cog_tween_pos_add(cog_pos2 pos, cog_pos2 dest_pos, double time) {
    cog_tween* tween = COG_STRUCT_MALLOC(cog_tween);
    tween->id = tweencnt++;
    tween->type = COG_T_POS;
    tween->pos = pos;
    tween->dest_pos = dest_pos;
    tween->time = time;
    tween->finished = false;
    cog_list_append(&active_tweens, (cog_dataptr) & (tween->id));
    cog_map_put(&tweens, tween->id, (void*) tween);
    return tween->id;
}

cog_tween* cog_tween_get(cog_tween_id id) {
    return (cog_tween*) cog_map_get(&tweens, id);
}

bool cog_tween_finished(cog_tween_id id) {
    cog_tween* tween = cog_tween_get(id);
    return tween->finished;
}

cog_pos2 cog_tween_pos_get(cog_tween_id id) {
    cog_tween* tween = cog_tween_get(id);
    cog_pos2 result = tween->pos;
    //cog_debugf("percent %lf ", tween->percent_progress);
    result.x += (tween->dest_pos.x - tween->pos.x) * tween->percent_progress;
    result.y += (tween->dest_pos.y - tween->pos.y) * tween->percent_progress;
    return result;
}

void cog_tween_removeall(void) {
    cog_list_removeall(&active_tweens);
}

void cog_tween_update(double timestep) {
    COG_LIST_FOREACH(&active_tweens) {
        cog_tween_id id = *((cog_tween_id*) curr->data);
        cog_tween* tween = cog_tween_get(id);
        if(tween->finished) {
            continue;
        }
        tween->time_progress += timestep;
        tween->percent_progress = cog_math_min(tween->time_progress / tween->time, 1.0);
        if(tween->time_progress > tween->time) {
            tween->finished = true;
        }
    }
}
