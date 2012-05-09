#include "cog.h"

typedef struct sprite
{
    cog_sprite* cogsprite;
} sprite;
static sprite;

void sprite_init(sprite* obj)
{
    obj->x = 0;
    obj->y = 0;
    obj->cogsprite = cog_add_anim("kitten.png", 10, 0, 1, 2, 3, 4,);
    cog_play_anim(obj->cogsprite);
}

void sprite_update(void* data)
{
    sprite* mysprite = (sprite*)data;
    obj->cogsprite.x += x;
    obj->cogsprite.y += y;
}

int main(void)
{
    cog_init();

    sprite_init(&sprite);
    cog_add_entity(&sprite, sprite_update);

    cog_mainloop();
    cog_destroy();
    return 0;
}
