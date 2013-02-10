#include "cog_anim.h"

#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"

static cog_anim_id animcnt;
static cog_map anims;
static cog_list active_anims; //anims drawn(active) at the moment

/*-----------------------------------------------------------------------------
 * Assumes animation is a single 1D animation frame.
 *-----------------------------------------------------------------------------*/
cog_anim_id cog_anim_add(char* img, cog_int n_frames)
{
    cog_anim* anim = COG_STRUCT_MALLOC(cog_anim);
    anim->id = animcnt++;
    cog_list_init(&anim->frames, sizeof(cog_sprite));
    anim->transition_millis = 0;
    anim->looped = COG_FALSE;
    anim->current_frame = 0;
    anim->current_frame_millis = 0;
    anim->paused = COG_FALSE;
    anim->n_frames = n_frames;
    anim->pos.x = 0;
    anim->pos.y = 0;
    anim->dim.w = 0;
    anim->dim.h = 0;
    anim->rot = 0;
    anim->vel.x = 0;
    anim->vel.y = 0;
    anim->finished = 0;

    cog_float w_frame = ((cog_float)1.0f / n_frames);
    cog_float h_frame = 1.0f;
    //Load nimages sprites in, with offset dependant on frame number.
    for(int i=0;i<n_frames;i++)
    {
        cog_sprite_add(img);
        cog_sprite_id sid = cog_sprite_add(img);
        cog_sprite* sprite = cog_sprite_get(sid);
        sprite->tex_pos.x = i*w_frame;
        sprite->tex_pos.y = 0.0f;
        sprite->tex_dim.w = w_frame;
        sprite->tex_dim.h = h_frame;
        cog_list_append(&(anim->frames), sprite);
    }
    cog_map_put(&anims, anim->id, (void*)anim);
    cog_list_append(&active_anims, (cog_dataptr)&(anim->id));
    return anim->id;
}

cog_bool cog_anim_collides_anim(cog_anim_id id0, cog_anim_id id1)
{
    cog_anim* anim0 = cog_anim_get(id0);
    cog_anim* anim1 = cog_anim_get(id1);
    if(cog_anim_dist_anim(id0, id1) <
            (anim0->dim.w + anim1->dim.w))
    {
        return COG_TRUE;
    }
    else
    {
        return COG_FALSE;
    }
}

cog_bool cog_anim_collides_sprite(cog_anim_id id0, cog_sprite_id id1)
{
    cog_anim* anim0 = cog_anim_get(id0);
    cog_sprite* sprite1 = cog_sprite_get(id1);
    if(cog_anim_dist_sprite(id0, id1) <
            (anim0->dim.w + sprite1->dim.w))
    {
        return COG_TRUE;
    }
    else
    {
        return COG_FALSE;
    }
}

cog_anim* cog_anim_get(cog_anim_id id)
{
    return (cog_anim*)cog_map_get(&anims, id);
}

void cog_anim_remove(cog_anim_id id)
{
    COG_LIST_FOREACH(&active_anims)
    {
        if(*((cog_anim_id*)curr->data) == id)
        {
            cog_list_remove(&active_anims, curr->data);
            break;
        }
    }
    cog_map_remove(&anims, id);
}

void cog_anim_removeall(void)
{
    cog_list_removeall(&active_anims);
}

void cog_anim_set_frames(cog_anim_id id, cog_int frame0, ...)
{
    cog_anim* anim = cog_anim_get(id);
    va_list ap;
    va_start(ap, frame0); 
    for(cog_int i=frame0;i>=0;i=va_arg(ap, cog_int))
    {
        cog_list_append(&(anim->frames), (cog_dataptr)&frame0);
//        cog_list_append(&(anim->frames), (cog_dataptr)&frame0);
    }
    va_end(ap);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
cog_float cog_anim_dist_anim(cog_anim_id id0, cog_anim_id id1)
{
    cog_anim* anim0 = cog_anim_get(id0);
    cog_anim* anim1 = cog_anim_get(id1);
    return cog_math_sqrt((anim0->pos.x - anim1->pos.x) * (anim0->pos.x - anim1->pos.x) +
        (anim0->pos.y - anim1->pos.y) * (anim0->pos.y - anim1->pos.y));
}

cog_float cog_anim_dist_sprite(cog_anim_id id0, cog_sprite_id id1)
{
    cog_anim* anim0 = cog_anim_get(id0);
    cog_sprite* sprite1 = cog_sprite_get(id1);
    return cog_math_sqrt((anim0->pos.x - sprite1->pos.x) * (anim0->pos.x - sprite1->pos.x) +
        (anim0->pos.y - sprite1->pos.y) * (anim0->pos.y - sprite1->pos.y));

}

void cog_anim_draw(void)
{
    //Draw anims
    COG_LIST_FOREACH(&active_anims)
    {
        //draw current sprite
        cog_anim* anim = cog_anim_get(*(cog_anim_id*)curr->data);
        if(anim->paused || anim->finished)
        {
            continue;
        }
        //find active frame to render
        cog_list* frame = (anim->frames.next);
        cog_uint i=0;
        while(i<anim->current_frame)
        {
            frame = frame->next;
            i++;
        }
        cog_sprite* sprite = frame->data;
        sprite->pos.x = anim->pos.x;
        sprite->pos.y = anim->pos.y;
        sprite->dim.w = anim->dim.w;
        sprite->dim.h = anim->dim.h;
        sprite->rot = anim->rot;
        cog_graphics_draw_sprite(sprite);
    }
}

void cog_anim_init()
{
    cog_map_init(&anims);
    cog_list_init(&active_anims, sizeof(cog_sprite_id));
}

void cog_anim_update(cog_uint delta_millis)
{
    COG_LIST_FOREACH(&active_anims)
    {
        //Draw current sprite
        cog_anim_id id = *((cog_anim_id*)curr->data);
        cog_anim* this_anim = (cog_anim*)cog_map_get(&anims,id);

        if(this_anim->paused)
        {
            continue;
        }
        this_anim->current_frame_millis += delta_millis;
        if(this_anim->current_frame_millis >= this_anim->transition_millis)
        {
            this_anim->current_frame++;
            this_anim->current_frame_millis = this_anim->current_frame_millis - this_anim->transition_millis; //Diff
            if(this_anim->current_frame >= this_anim->n_frames)
            {
                if(this_anim->looped)
                {
                    this_anim->current_frame = 0;
                }
                else
                {
                    this_anim->finished = COG_TRUE;
                }
            }
        }
    }

    //Do physics
    COG_LIST_FOREACH(&active_anims)
    {
        cog_anim* this_anim = cog_anim_get(*(cog_anim_id*)curr->data);
        this_anim->pos.x += delta_millis * this_anim->vel.x;
        this_anim->pos.y += delta_millis * this_anim->vel.y;
    }
}
