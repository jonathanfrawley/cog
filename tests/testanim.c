#include "cog.h"
#include "cog_anim.h"
#include "cog_math.h"

typedef struct entity
{
    cog_anim_id anim;
} entity;

void entity_init(entity* obj)
{
    obj->anim = cog_anim_add("../media/kitten_anim.png", 3);
    cog_anim* anim = cog_anim_get(obj->anim);
    anim->transition_millis = 150;
    anim->looped = COG_TRUE;
    anim->x = cog_screenw() * 0.5f;
    anim->y = cog_screenh() * 0.5f;
    anim->w = 256;
    anim->h = 256;
    anim->rot = 0;
    anim->paused = COG_FALSE;
    cog_anim_set_frames(obj->anim, 0, 1, 2);
}

void entity_update(entity* obj)
{
    //Do some simple movement
    cog_anim* anim = cog_anim_get(obj->anim);
    anim->x += 0.001;
    anim->rot += (COG_PI * 0.0001);
}

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);

    entity myentity;
    entity_init(&myentity);

    while(! cog_hasquit())
    {
        entity_update(&myentity);
        cog_loopstep();
    }

    return 0;
}
