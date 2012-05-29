#include "cog.h"

typedef struct sprite
{
    cog_sprite_id sprite;
} sprite;
static sprite;

void sprite_init(sprite* obj)
{
    obj->x = 0;
    obj->y = 0;
    obj->sprite = cog_add_anim("../media/kitten_anim.png", 10, 1, 3, 0, 1, 2);
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
