#include "cog.h"
#include "cog_anim.h"
#include "cog_math.h"

static int current_x = 20;
static int N_ENTITIES = 10;

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
    anim->x = current_x;
    current_x += 200;
    anim->y = cog_screenh() * 0.5f;
    anim->w = 128;
    anim->h = 128;
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
    cog_init();

    entity myentity[N_ENTITIES];
    for(int i=0;i<N_ENTITIES;i++) 
    {
        entity_init(&myentity[i]);
    }

    while(! cog_hasquit())
    {
        for(int i=0;i<N_ENTITIES;i++) 
        {
            entity_update(&myentity[i]);
        }
        cog_loopstep();
    }

    return 0;
}
