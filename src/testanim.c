#include "cog.h"

typedef struct entity
{
    cog_anim_id anim;
} entity;

void entity_init(entity* obj)
{
    obj->anim = cog_anim_add(
            "../media/kitten_anim.png",
            10, //transition
            COG_TRUE, //looped
            3, //nimages
            10, //x
            10, //y
            128, //w
            128, //h
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
            200,
            200);
}

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);

    entity myentity;
    entity_init(&myentity);
//    cog_add_entity(&myentity, entity_update);

//    cog_mainloop();
    while(! cog_hasquit())
    {
        entity_update(&myentity);
        cog_loopstep();
    }

    return 0;
}
