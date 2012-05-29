#ifndef  COG_TYPES_H
#define  COG_TYPES_H

//types
typedef float cog_float;
typedef int cog_sprite_id;
typedef int cog_anim_id;
typedef unsigned cog_snd_id;
typedef unsigned cog_uint;
typedef int cog_bool;
typedef void* cog_dataptr;
typedef unsigned cog_uint;

//mm
inline void *cog_malloc(cog_uint size)
{
    void* ptr = (void*)malloc(size);
    if(ptr == 0)
    {   
        assert(0 && "dynamic memory allocation failed");
        return 0;
    }   
    return ptr;
}
inline void cog_free(void* ptr)
{
    free(ptr);
}


#endif   // COG_TYPES_H
