#include "cog.h"

typedef struct entity
{
    cog_anim_id anim;
} entity;

void anim_init(entity* obj)
{
    obj->anim = cog_anim_add("../media/kitten_anim.png",
            10,
            1,
            3,
            10,
            10,
            32,
            32,
            0, 1, 2);
    cog_anim_play(obj->anim);
}

void entity_update(entity* obj)
{
    //entity* myentity = (entity*)data;
    //cog_anim* anim = cog_get_anim(myentity->cog_anim_id);
    //Do some simple movement
    //anim->x += 2;
    //anim->y += 2;
    cog_anim_update_pos(obj->anim,
            cog_anim_getx(obj->anim)+2,
            cog_anim_gety(obj->anim)+2);
}

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);

    entity myentity;
    anim_init(&myentity);
//    cog_add_entity(&myentity, entity_update);

//    cog_mainloop();
    while(! cog_hasquit())
    {
        entity_update(&myentity);
        cog_loopstep();
    }

    return 0;
}
