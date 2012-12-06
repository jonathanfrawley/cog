#include "cog_anim.h"

#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_map.h"
#include "cog_sprite.h"

static cog_anim_id animcnt;
static cog_map anims;
static cog_list activeanims; //anims drawn(active) at the moment

/*-----------------------------------------------------------------------------
 * Assumes animation is a single 1D animation frame.
 *-----------------------------------------------------------------------------*/
cog_anim_id cog_anim_add(char* animimg, cog_int nframes)
{
    cog_anim* anim = COG_STRUCT_MALLOC(cog_anim);
    anim->id = animcnt++;
    cog_list_init(&anim->frames, sizeof(cog_sprite));
    anim->transition_millis = 0;
    anim->looped = COG_FALSE;
    anim->currentframe = 0;
    anim->currentframe_millis = 0;
    anim->paused = COG_FALSE;
    anim->nframes = nframes;
    anim->x = 0;
    anim->y = 0;
    anim->w = 0;
    anim->h = 0;
    anim->rot = 0;
    anim->xvel = 0;
    anim->yvel = 0;
    anim->finished = 0;

    cog_float wanimframe = ((cog_float)1.0 / nframes);
    cog_float hanimframe = 1.0;
    //Load nimages sprites in, with offset dependant on frame number.
    for(int i=0;i<nframes;i++)
    {
        cog_sprite_id sid = cog_sprite_add(animimg);
        cog_sprite* sprite = cog_sprite_get(sid);
        sprite->texx = i*wanimframe;
        sprite->texy = 0.0f;
        sprite->texw = wanimframe;
        sprite->texh = hanimframe;
        cog_list_append(&(anim->frames), sprite);
    }
    cog_map_put(&anims, anim->id, (void*)anim);
    cog_list_append(&activeanims, (cog_dataptr)&(anim->id));

    return anim->id;
}

cog_anim* cog_anim_get(cog_anim_id id)
{
    return (cog_anim*)cog_map_get(&anims, id);
}

void cog_anim_remove(cog_anim_id id)
{
    COG_LIST_FOREACH(&activeanims)
    {
        if(*((cog_anim_id*)curr->data) == id)
        {
            cog_list_remove(&activeanims, curr->data);
            break;
        }
    }
    cog_map_remove(&anims, id);
}

void cog_anim_removeall(void)
{
    cog_list_removeall(&activeanims);
}

void cog_anim_set_frames(cog_anim_id id, cog_int frame0, ...)
{
    cog_anim* anim = cog_anim_get(id);
    va_list ap;
    va_start(ap, frame0); 
    for(cog_int i=frame0;i>=0;i=va_arg(ap, cog_int))
    {
        cog_list_append(&(anim->frames), (cog_dataptr)&frame0);
    }
    va_end(ap);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_anim_draw(void)
{
    //Draw anims
    COG_LIST_FOREACH(&activeanims)
    {
        //draw current sprite
        cog_anim* anim = cog_anim_get(*(cog_anim_id*)curr->data);
        if(anim->paused)
        {
            continue;
        }
        //find active frame to render
        cog_list* frame = (anim->frames.next);
        cog_uint i=0;
        while(i<anim->currentframe)
        {
            frame = frame->next;
            i++;
        }
        cog_sprite* sprite = frame->data;
        sprite->x = anim->x;
        sprite->y = anim->y;
        sprite->w = anim->w;
        sprite->h = anim->h;
        sprite->rot = anim->rot;
        sprite->xvel = anim->xvel;
        sprite->yvel = anim->yvel;
        cog_graphics_draw_sprite(sprite);
    }
}

void cog_anim_init()
{
    cog_map_init(&anims);
    cog_list_init(&activeanims, sizeof(cog_sprite_id));
}

void cog_anim_update(cog_uint deltamillis)
{
    COG_LIST_FOREACH(&activeanims)
    {
        //Draw current sprite
        cog_anim_id id = *((cog_anim_id*)curr->data);
        cog_anim* thisanim = (cog_anim*)cog_map_get(&anims,id);

        if(thisanim->paused)
        {
            continue;
        }
        thisanim->currentframe_millis += deltamillis;
        if(thisanim->currentframe_millis >= thisanim->transition_millis)
        {
            thisanim->currentframe++;
            thisanim->currentframe_millis = thisanim->currentframe_millis - thisanim->transition_millis; //Diff
            if(thisanim->currentframe >= thisanim->nframes)
            {
                if(thisanim->looped)
                {
                    thisanim->currentframe = 0;
                }
                else
                {
                    thisanim->finished = COG_TRUE;
                }
            }
        }
    }

    //Do physics
    COG_LIST_FOREACH(&activeanims)
    {
        cog_anim* thisanim = cog_anim_get(*(cog_anim_id*)curr->data);
        thisanim->x += deltamillis * thisanim->xvel;
        thisanim->y += deltamillis * thisanim->yvel;
    }
}
