
//types
typedef float cog_float;
typedef int cog_sprite_id;
typedef int cog_anim_id;
typedef unsigned cog_uint;

//main
void cog_init();
void cog_mainloop();
void cog_destroy();

//anim
cog_anim_id cog_add_anim(char* animimg, cog_uint transition_millis, ...);
void cog_play_anim(cog_anim_id id);
