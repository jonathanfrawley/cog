#include "cog_sprite.h"

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"

static cog_list activesprites; //sprites drawn(active) at the moment
static cog_map sprites;
static cog_sprite_id spritecnt;

cog_sprite_id cog_sprite_add(char* img)
{
    cog_sprite* sprite = COG_STRUCT_MALLOC(cog_sprite);
    sprite->id = spritecnt++;
    sprite->texid = cog_graphics_load_texture(img);
    sprite->x = 0;
    sprite->y = 0;
    sprite->w = 0;
    sprite->h = 0;
    sprite->rot = 0;
    sprite->texx = 0.0f;
    sprite->texy = 0.0f;
    sprite->texw = 1.0f;
    sprite->texh = 1.0f;
    //vel always 0 to start with
    sprite->xvel = 0.0f;
    sprite->yvel = 0.0f;
    cog_map_put(&sprites, sprite->id, (void*)sprite);
    return sprite->id;
}

cog_float cog_sprite_anim_dist(cog_sprite_id a, cog_anim_id b)
{
    cog_sprite* asprite = cog_sprite_get(a);
    cog_anim* banim = cog_anim_get(b);
    cog_sprite* bsprite = (cog_sprite*)banim->frames.next->data;
    return cog_math_sqrt((asprite->x - bsprite->x) * (asprite->x - bsprite->x) +
        (asprite->y - bsprite->y) * (asprite->y - bsprite->y));
}

cog_float cog_sprite_dist(cog_sprite_id a, cog_sprite_id b)
{
    cog_sprite* asprite = cog_sprite_get(a);
    cog_sprite* bsprite = cog_sprite_get(b);
    return cog_math_sqrt((asprite->x - bsprite->x) * (asprite->x - bsprite->x) +
        (asprite->y - bsprite->y) * (asprite->y - bsprite->y));
}

cog_sprite* cog_sprite_get(cog_sprite_id id)
{
    return (cog_sprite*)cog_map_get(&sprites, id);
}

void cog_sprite_remove(cog_sprite_id id)
{
    COG_LIST_FOREACH(&activesprites)
    {
        if(*((cog_sprite_id*)curr->data) == id)
        {
            cog_list_remove(&activesprites, curr->data);
            break;
        }
    }
    cog_map_remove(&sprites, id);
}

void cog_sprite_removeall(void)
{
    cog_list_removeall(&activesprites);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_sprite_init(void)
{
    cog_map_init(&sprites);
    cog_list_init(&activesprites, sizeof(cog_sprite_id));
}

void cog_sprite_draw(void)
{
    //Draw sprites
    COG_LIST_FOREACH(&activesprites)
    {
        //draw current sprite
        cog_sprite* thissprite = (cog_sprite*)cog_map_get(&sprites,*((cog_sprite_id*)curr->data));
        cog_graphics_draw_sprite(thissprite);
    }
}

void cog_sprite_update(cog_float timedelta)
{
    COG_LIST_FOREACH(&activesprites)
    {
        cog_sprite* thissprite = (cog_sprite*)cog_map_get(&sprites,*((cog_sprite_id*)curr->data));
        //do physics update for current sprite
        thissprite->x += timedelta * thissprite->xvel;
        thissprite->y += timedelta * thissprite->yvel;
    }
}
